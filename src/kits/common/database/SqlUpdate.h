#pragma once
#include "SqlQuery.h"
#include "SqlTypes.h"
#include "kits/common/serialization/serialize_qvariantmap.h"
#include <QHash>

namespace _Kits
{
    template <typename T, typename DBDriver = PostgreTag>
    class SqlUpdate : public SqlQuery<T, DBDriver>
    {
      public:
        SqlUpdate() = default;
        ~SqlUpdate() = default;

        // 单条更新设置
        SqlUpdate<T> &set(const QString &field, const QVariant &value)
        {
            if (!field.isEmpty())
            {
                m_setValues[field] = value;
            }
            return *this;
        }

        SqlUpdate<T> &set(const QVariantHash &values)
        {
            for (auto it = values.constBegin(); it != values.constEnd(); ++it)
            {
                set(it.key(), it.value());
            }
            return *this;
        }

        SqlUpdate<T> &where(const QString &field,
                            OperatorComparison op,
                            const QVariant &value,
                            OperatorLogical logicOperator = OperatorLogical::And)
        {
            return addCondition(field, op, value, logicOperator);
        }

        // 批量更新入口
        template <typename Container>
        SqlUpdate<T> &batchUpdate(const Container &objects)
        {
            if (objects.empty())
                return *this;

            m_isBatch = true;
            m_batchSize = objects.size();
            QStringList validFields = T::getFieldNames();

            // 收集批量数据
            for (const auto &obj : objects)
            {
                auto map = _Kits::toVariantMap(obj);

                // 处理SET参数
                for (auto it = map.begin(); it != map.end(); ++it)
                {
                    if (validFields.contains(it.key()) && !m_conditions.contains(it.key()))
                    { // 排除WHERE字段
                        m_batchSetValues[":set_" + it.key()] << it.value();
                    }
                }

                // 处理WHERE参数
                for (const auto &cond : m_conditions)
                {
                    if (map.contains(cond.field))
                    {
                        m_batchWhereValues[":" + cond.field] << map.value(cond.field);
                    }
                }
            }
            return *this;
        }

        // 统一执行入口
        bool exec()
        {
            if (m_isBatch)
            {
                return execBatch();
            }
            return execSingle();
        }

        int getNumAffected() const
        {
            if (!this->m_success)
                return 0;
            return m_isBatch ? m_batchSize : this->m_query.numRowsAffected();
        }

      private:
        SqlUpdate<T> &addCondition(const QString &field, OperatorComparison op, const QVariant &value, OperatorLogical logicOp)
        {
            m_conditions.append(Condition(field, op, value, logicOp));
            return *this;
        }
        // 单条更新执行
        bool execSingle()
        {
            if (m_setValues.isEmpty())
            {
                qWarning() << "Update fields cannot be empty";
                return false;
            }

            this->m_sql = buildUpdateStatement();

            if (!this->m_query.prepare(this->m_sql) || !bindValues() || !this->m_query.exec())
            {
                this->innerError();
                return false;
            }

            this->m_success = true;
            return true;
        }

        // 批量更新执行
        bool execBatch()
        {
            auto &db = this->m_db;
            if (!db.transaction())
            { // 显式检查事务启动
                qCritical() << "Failed to start transaction:" << db.lastError().text();
                return false;
            }

            if (m_batchSetValues.isEmpty() || m_batchWhereValues.isEmpty())
            {
                qWarning() << "Batch data is incomplete";
                return false;
            }

            this->m_sql = buildUpdateStatement();
            if (!this->m_query.prepare(this->m_sql))
            {
                this->innerError();
                return false;
            }

            // 批量绑定参数
            for (auto it = m_batchSetValues.begin(); it != m_batchSetValues.end(); ++it)
            {
                this->m_query.bindValue(it.key(), it.value());
            }
            for (auto it = m_batchWhereValues.begin(); it != m_batchWhereValues.end(); ++it)
            {
                this->m_query.bindValue(it.key(), it.value());
            }

            if (!this->m_query.execBatch())
            {
                db.rollback();
                this->innerError();
                return false;
            }

            this->m_success = db.commit();
            return this->m_success;
        }

      private:
        struct Condition
        {
            QString field;
            OperatorComparison op;
            QVariant value;
            OperatorLogical logicOperator = OperatorLogical::And; // 默认值统一

            Condition(const QString &f, OperatorComparison o, const QVariant &v, OperatorLogical lo = OperatorLogical::And)
                : field(f), op(o), value(v), logicOperator(lo)
            {
            }
        };

        QString buildUpdateStatement() const
        {
            return QString("UPDATE %1 SET %2%3;")
                .arg(T::tableName())
                .arg(buildSetClause())
                .arg(buildWhereClause().isEmpty() ? "" : " WHERE " + buildWhereClause());
        }

        QString buildSetClause() const
        {
            QStringList clauses;
            clauses.reserve(m_setValues.size());

            for (auto it = m_setValues.keyBegin(); it != m_setValues.keyEnd(); ++it)
            {
                clauses << QString("%1 = :set_%1").arg(*it);
            }
            return clauses.join(", ");
        }

        QString buildWhereClause() const
        {
            QStringList clauses;
            for (const auto &condition : m_conditions)
            {
                QString clause = QString("%1 %2 :%1").arg(condition.field).arg(SqlOperators::comparisonOperatorMap.at(condition.op));

                if (!clauses.isEmpty())
                {
                    clause.prepend(SqlOperators::logicalOperatorMap.at(condition.logicOperator) + " ");
                }
                clauses << clause;
            }
            return clauses.join(" ");
        }

        //** 新增统一化的参数绑定方法 **
        bool bindValues()
        {
            for (auto it = m_setValues.constBegin(); it != m_setValues.constEnd(); ++it)
            {
                this->m_query.bindValue(":set_" + it.key(), it.value());
            }

            for (const auto &condition : m_conditions)
            {
                this->m_query.bindValue(":" + condition.field, condition.value);
            }
            return true;
        }

        bool m_isBatch = false;
        int m_batchSize = 0;
        QHash<QString, QVariantList> m_batchSetValues;
        QHash<QString, QVariantList> m_batchWhereValues;
        QHash<QString, QVariant> m_setValues; // SET字段组
        QList<Condition> m_conditions;        // WHERE条件组
    };
} // namespace _Kits