#ifndef NINVENTORYCONTAINER_H
#define NINVENTORYCONTAINER_H
//------------------------------------------------------------------------------
/**
    @class nInventoryContainer
    
    Implements an item container for entities
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/ncmdprotonativecpp.h"
#include "kernel/nobject.h"

#include "util/narray.h"

//------------------------------------------------------------------------------
class nInventoryItem;
class nEntityObject;
#ifndef NGAME
class nGfxServer2;
#endif//!NGAME

//------------------------------------------------------------------------------
class nInventoryContainer : public nObject
{
public:
    enum WeaponSlotType
    {
        WST_ANY = -1,

        WST_MELEE = 0,
        WST_MAIN = 1,
        WST_SECONDARY = 2,
        WST_HANDGUN = 3,
        WST_CARRIED = 4,
        WST_LAST
    };

    static const int INFINITE_SLOTS = -1;

    /// constructor
    nInventoryContainer();

    /// destructos
    ~nInventoryContainer();

    /// set size of the container
    void SetSize( const int newSize );

        /// get the number of items
        int GetNumItems ()const;
        /// get the weapon item of one slot
        nInventoryItem * GetWeaponItemAt(int);
        /// get a normal item
        nInventoryItem * GetItemAt(int);
        /// get an item with an id
        nInventoryItem * GetItem(int);
        /// get num bullets from an ammunition type
        int GetNumBullets(int)const;
        /// remove bullets from inventory
        void RemoveBullets(int, int);
        /// get num equipment objects
        int GetNumEquipment(const char *)const;
        /// remove equipment from inventory
        void RemoveEquipment(const char*, int);
        /// get entity slot
        int GetEntitySlot(nEntityObject*)const;
        /// get equipment slot
        int GetEquipmentSlot(const char *)const;
        /// get equipment item
        nEntityObject* GetInventoryItem(const char *)const;

    /// insert an item in the inventory
    bool InsertItem( nInventoryItem * item, WeaponSlotType slotType = WST_ANY );

    /// remove the weapon item of one slot
    nInventoryItem * RemoveWeaponItemAt( WeaponSlotType slot );
    /// remove a normal item
    nInventoryItem * RemoveItemAt( int item );
    /// get the item with an nEntityObject
    nInventoryItem * GetItemWith( const nEntityObject * const entity );
    /// get the entity of a type
    nEntityObject * GetEntityWith( const char * name );

#ifndef NGAME
    /// debug draw of entity
    void DebugDraw( nGfxServer2 * const gfxServer );
#endif//!NGAME

private:
    static const int INITIAL_SIZE = 10;

    static int slotRestrictions[];
    static const char * slotNames[];

    /// empty the inventory
    void Clear();
    void ClearSlots();

    /// insert a weapon in the inventory
    bool InsertWeaponItem( nInventoryItem * item, WeaponSlotType slotType );
    /// insert a normal item in the inventory
    bool InsertNormalItem( nInventoryItem * item );

    nInventoryItem * weaponSlots[ WST_LAST ];

    nArray<nInventoryItem*> container;

    int maxStackSize;

    bool isPlayerInventory;
};

//------------------------------------------------------------------------------
/**
    @returns the number of items
*/
inline
int
nInventoryContainer::GetNumItems()const
{
    return this->container.Size();
}

//------------------------------------------------------------------------------
#endif//NCINVENTORYCONTAINER_H
