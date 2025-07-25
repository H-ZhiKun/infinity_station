#include "JiHeDataBaseOpt.h"
#include "kits/common/log/CRossLogger.h"

using namespace _Kits::_Orm;
using namespace _Kits;
namespace _Modules
{

    bool JiHeDataBaseOpt::DataBaseInsert(const std::vector<jihe_data> &vecData)
    {
        if (!vecData.empty())
        {
            _Kits::SqlInsert<jihe_data> insert;
            insert.insert(vecData).exec();

            int nums = insert.getNumAffected();
            _Kits::LogDebug("JiHe data end nums={}", nums);
            return nums > 0;
        }
        return false;
    }

} // namespace _Modules