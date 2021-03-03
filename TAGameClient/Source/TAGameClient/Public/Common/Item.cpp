#include "Common/Item.h"
#include "Common/GameData.h"
#include "Common/ItemDetail.h"



namespace ta
{
    Item Item::InvalidItem;

    Item* Item::createItem(const ItemGameData* data, const int32 stackCount) noexcept
    {
        if (stackCount <= 0)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return &Item::InvalidItem;
        }

        if (nullptr == data)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return &Item::InvalidItem;
        }

        ItemDetail temp;
        if (false == Item::generateRandomItemDetail(data, temp))
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return &Item::InvalidItem;
        }

        Item* rv = new Item(data, temp, stackCount);

        return rv;
    }

    Item* Item::createItem(const ItemGameData* data, const ItemDetail& detail, const int32 stackCount) noexcept
    {
        if (stackCount <= 0)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return &Item::InvalidItem;
        }

        if (nullptr == data)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return &Item::InvalidItem;
        }

        Item* rv = new Item(data, detail, stackCount);

        return rv;
    }

    Item* Item::copyItems_(const Item* targetItem, const int32 stackCount) noexcept
    {
        if (stackCount <= 0)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return &Item::InvalidItem;
        }

        if (false == targetItem->checkValid_())
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return &Item::InvalidItem;
        }

        Item* rv = new Item(*targetItem);
        rv->setStackCount_(stackCount);

        return rv;
    }

    Item* Item::divideItemStackCount_(Item*& targetItem, const int32 count) noexcept
    {
        if (count <= 0)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return &(Item::InvalidItem);
        }

        if (false == targetItem->checkValid_())
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return &(Item::InvalidItem);
        }

        const int32 currentStackCount = targetItem->getStackCount_();
        const int32 finalCount = currentStackCount < count ? currentStackCount : count;

        targetItem->setStackCount_(currentStackCount - finalCount);
        if (currentStackCount == finalCount) // 다썼으면 비워줘야한다.
        {
            targetItem = &Item::InvalidItem;
        }

        Item* rv = new Item(*targetItem);
        rv->setStackCount_(finalCount);
        
        return rv;
    }

    Item::~Item(void) noexcept
    {
    }

    bool Item::initializeItem_(const ItemGameData* data, const ItemDetail& detail, const int32 stackCount) noexcept
    {
        if (stackCount <= 0)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return &Item::InvalidItem;
        }

        if (nullptr == data)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return &Item::InvalidItem;
        }

        _base = data;
        _detail = detail;
        _stackCount = stackCount;
        _isValid = true;

        return true;
    }

    bool Item::checkValid_(void) const noexcept
    {
        return _isValid;
    }

    void Item::clear_(void) noexcept
    {
        //_stackCount = 0;
    }

    int32 Item::getStackCount_(void) const noexcept
    {
        return _stackCount;
    }

    bool Item::setStackCount_(const int32 stackCount) noexcept
    {
        if (stackCount < 0)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            _stackCount = 0;
            return false;
        }

        _stackCount = stackCount;
        return true;
    }

    const ItemGameData* Item::getBase_(void) const noexcept
    {
        return _base;
    }

    const ItemDetail& Item::getDetail_(void) const noexcept
    {
        return _detail;
    }

    Item::Item(void) noexcept
        : _isValid(false)
        , _base(nullptr)
        , _stackCount(0)
    {
        // clear_();
    }

    Item::Item(const ItemGameData* data, const ItemDetail& detail, const int32 stackCount /*= 1*/) noexcept
        : _isValid(true)
        , _base(data)
        , _detail(detail)
        , _stackCount(stackCount)
    {
        // clear_();
        // 아이템 정보 초기화
    }

    Item::Item(const Item& item) noexcept
        : _isValid(item._isValid)
        , _base(item._base)
        , _detail(item._detail)
        , _stackCount(item._stackCount)
    {
        TA_ASSERT_DEV(true == _isValid, "유효하지 않은 아이템을 복사?");
    }
    
    bool Item::generateRandomItemDetail(const ItemGameData* data, ItemDetail& detail) noexcept
    {
        if (nullptr == data)
        {
            return false;
        }
        // 랜덤 세부정보 세팅

        return true;
    }
}
