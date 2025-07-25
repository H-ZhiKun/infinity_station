
# **📌 RESTful API 文档**

## **1. 数据库操作 API（Database）**

- **接口地址**：`http://<ip>:<port>/api/database`
- **请求方式**：`POST`
- **请求格式**：`application/json`
- **返回格式**：`application/json`
- **接口功能**：提供 **查询（Select）** 操作，支持 **条件筛选、排序、分页**。

### **1.1 请求参数**

#### **1.1.1 公共参数**

| 参数         | 类型     | 是否必填 | 说明                      |
| ------------ | -------- | -------- | ------------------------- |
| `method`     | `string` | ✅ 是     | 操作类型，固定为 `select` |
| `table`      | `string` | ✅ 是     | 目标数据库表名            |
| `filters`    | `array`  | ❌ 否     | 查询时的筛选条件          |
| `sort`       | `object` | ❌ 否     | 查询时的排序字段          |
| `pagination` | `object` | ❌ 否     | 查询时的分页参数          |

#### **1.1.2 `filters` 结构**

用于 `select` 操作，支持 `AND` 逻辑查询。

| 参数       | 类型     | 是否必填 | 说明                                                         |
| ---------- | -------- | -------- | ------------------------------------------------------------ |
| `field`    | `string` | ✅ 是     | 需要筛选的字段名                                             |
| `value`    | `any`    | ✅ 是     | 过滤条件的值                                                 |
| `operator` | `string` | ✅ 是     | 逻辑运算符，支持：`=`、`!=`、`>`、`>=`、`<`、`<=`            |
| `logic`    | `string` | ❌ 否     | 逻辑运算符，支持：AND（默认）和 OR，影响相邻后一条过滤信息。 |

#### **1.1.3 `sort` 结构**

用于 `select` 操作，支持排序规则。

| 参数    | 类型     | 是否必填 | 说明                                          |
| ------- | -------- | -------- | --------------------------------------------- |
| `field` | `string` | ✅ 是     | 需要排序的字段名                              |
| `order` | `string` | ✅ 是     | 排序方式，可选：`asc`（升序）、`desc`（降序） |

#### **1.1.4 `pagination` 结构**

用于 `select` 操作，支持分页查询，避免返回过多数据影响性能。

| 参数     | 类型  | 是否必填 | 说明                                                                       |
| -------- | ----- | -------- | -------------------------------------------------------------------------- |
| `limit`  | `int` | ✅ 是     | 每页返回的最大记录数，例如 `10` 表示每次最多返回 10 条数据                 |
| `offset` | `int` | ✅ 是     | 需要跳过的记录数，例如 `0` 表示从第一条数据开始，`10` 表示跳过前 10 条数据 |

---

### **1.2 使用示例**

#### **1.2.1 查询数据（Select）**

**请求**
```json
{
    "method": "select",
    "table": "users",
    "filters": [
        {
            "field": "age",
            "value": 30,
            "operator": ">",
            "logic": "OR" //影响相邻后一条过滤信息。
        },
        {
            "field": "name",
            "value": "Alice",
            "operator": "="
        }
    ],
    "pagination": {
        "limit": 10,
        "offset": 0
    },
    "sort": {
        "field": "name",
        "order": "asc"
    }
}
```

**返回**
```json
{
    "status": "success",
    "code": 200,
    "data": [
        {
            "name": "Alice",
            "age": 30,
            "email": "alice@example.com"
        }
    ]
}
```

---

## **2. 消息通信 API（Communication）**

- **接口地址**：`http://<ip>:<port>/api/communication`
- **请求方式**：`POST`
- **请求格式**：`application/json`
- **返回格式**：`application/json`
- **接口功能**：实现不同系统之间的数据通信。

### **2.1 请求参数**

| 参数       | 类型     | 是否必填 | 说明                                               |
| ---------- | -------- | -------- | -------------------------------------------------- |
| `consumer` | `string` | ✅ 是     | 通信目标，可选：`shidai`、`tangche`                |
| `type`     | `string` | ✅ 是     | 数据类型，可选：`realtime`、`file`、`网检`、`网巡` |
| `data`     | `object` | ✅ 是     | 传输的数据内容                                     |

#### **2.1.1 `data` 结构**

| 参数        | 类型     | 是否必填 | 说明           |
| ----------- | -------- | -------- | -------------- |
| `msg`       | `string` | ❌ 否     | 当前正文       |
| `file_path` | `string` | ❌ 否     | 可选，文件路径 |

---

### **2.2 使用示例**

#### **2.2.1 发送实时消息（Realtime）**

**请求**
```json
{
    "consumer": "shidai",
    "type": "realtime",
    "data": {
        "msg": "This is a real-time message."
    }
}
```

**返回**
```json
{
    "status": "success",
    "code": 200,
    "message": "Message sent successfully"
}
```

---

#### **2.2.2 发送文件消息（File）**

**请求**
```json
{
    "consumer": "tangche",
    "type": "file",
    "data": {
        "file_path": "/path/to/file.txt"
    }
}
```

**返回**
```json
{
    "status": "success",
    "code": 200,
    "message": "File sent successfully"
}
```

---

## **3. TCP 订阅推送服务 API（Subscription & Push）**

- **服务器地址**：`tcp://<ip>:<port>`
- **请求方式**：TCP 连接
- **数据格式**：  
  - **请求**：`{[(tcp_header)]}int64请求正文{[(tcp_tail)]}`
  - **推送**：`{[(tcp_header)]}int64推送正文{[(tcp_tail)]}`

#### **3.1 订阅操作**

**请求**
```json
{
    "type": "subscribe",
    "category": "task",
    "data": {
        "msg": "接收任务通知"
    }
}
```

**返回**
```json
{
    "status": "success",
    "code": 200,
    "message": "Subscribed to task notifications"
}
```

---

#### **3.2 取消订阅**

**请求**
```json
{
    "type": "unsubscribe",
    "category": "task",
    "data": {
        "msg": "取消任务通知"
    }
}
```

**返回**
```json
{
    "status": "success",
    "code": 200,
    "message": "Unsubscribed from task notifications"
}
```

---

#### **3.3 发布消息**

**请求**
```json
{
    "type": "publish",
    "category": "task",
    "data": {
        "msg": "新的任务发布"
    }
}
```

**返回**
```json
{
    "status": "success",
    "code": 200,
    "message": "Message published successfully"
}
```

---
