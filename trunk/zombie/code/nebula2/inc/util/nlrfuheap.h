#ifndef N_LRFUHEAP_H
#define N_LRFUHEAP_H
//------------------------------------------------------------------------------
/**
    @class nLRFUHeap

    Least Recently/Frequetly Used Heap, implemented using the article:

    "On the Existence of a Spectrum of Policies that Subsumes the Least Recently
    Used (LRU) and Least Frequently Used (LFU) Policies", Donghee Lee 1999

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
template<class TYPE> class nLRFUHeap {
public:
    enum
    {
        InvalidIndex = -1
    };

    /// constructor
    nLRFUHeap( int size );

    /// clear the heap
    void Clear();

    /// return the number of elements in the heap
    int Size()const{ return this->num; }

    /// search in the heap using a function to discriminate nodes
    /// if an element is founded its crf value is updates
    template<class Function>int Search(Function & fun)
    {
        // search first the most used nodes
        for( int i = (num - 1) ; i >= 0 ; --i )
        {
            if( fun( this->heap[ i ]->data ) )
            {
                float tc = (float)nKernelServer::Instance()->GetTimeServer()->GetTime();

                Node * node = this->heap[ i ];

                node->crf = this->NodeWeight( 0 ) + this->NodeWeight( tc - node->last ) * node->crf;
                node->last = tc;

                this->Restore( i );

                return i;
            }
        }

        return InvalidIndex;
    }

    /// return an element of the heap
    TYPE& operator[]( int index );
    /// insert a new element in the heap
    TYPE Insert( TYPE & elm );
    /// remove an element from the heap
    TYPE Remove( int index );
private:
    /// Least Recently/Frequently Used node
    struct Node{
        float crf;  /// Combined Recency and Frequency value
        float last; /// last accessed time
        TYPE data;
    };

    /// weighing functions of LRFU
    float NodeWeight( float t )const;
    /// replace the oldest node for a new node
    Node * ReplaceRoot( Node * node );
    /// restore the position of a node in the heap
    void Restore( int index );
    /// swap two nodes
    void SwapNodes( int i1, int i2 );

    /// get the left child index
    static int LeftNode( int index );
    /// get the right child index
    static int RightNode( int index );
    /// get the parent index
    static int ParentNode( int index );

    /// check if exist a node in the index
    bool ExistNode( int index )const;

    nFixedArray< Node* > heap;
    int num;
};

//------------------------------------------------------------------------------
/**
    @param size fixed size of the heap
*/
template<class TYPE>
nLRFUHeap<TYPE>::nLRFUHeap( int size ):num( 0 )
{
    this->heap.SetSize( size );
}

//------------------------------------------------------------------------------
/**
*/
template<class TYPE>
void
nLRFUHeap<TYPE>::Clear()
{
    for( int i = 0 ; i < this->num ; ++i )
    {
        n_delete( this->heap[ i ] );
    }

    this->num = 0;
}

//------------------------------------------------------------------------------
/**
    @param index node index
    @returns node data
*/
template<class TYPE>
TYPE&
nLRFUHeap<TYPE>::operator[]( int index )
{
    return this->heap[index]->data;
}

//------------------------------------------------------------------------------
/**
    @param index node index
    @returns left child node
*/
template<class TYPE>
int
nLRFUHeap<TYPE>::LeftNode( int index ){
    return ( index * 2 + 1 );
}

//------------------------------------------------------------------------------
/**
    @param index node index
    @returns right child node
*/
template<class TYPE>
int
nLRFUHeap<TYPE>::RightNode( int index ){
    return ( index * 2 + 2 );
}

//------------------------------------------------------------------------------
/**
    @param index node index
    @returns parent child node
*/
template<class TYPE>
int
nLRFUHeap<TYPE>::ParentNode( int index ){
    return ( index >> 1 );
}

//------------------------------------------------------------------------------
/**
    @param time time elapsed
    @return the weighing value
*/
template<class TYPE>
float
nLRFUHeap<TYPE>::NodeWeight( float time )const
{
    // CRF reduced to half every  1/0.01 seconds
    return n_pow( 0.5, 0.01 * time );
}

//------------------------------------------------------------------------------
/**
    @param index node index
    @returns true if there is a node with that index
*/
template<class TYPE>
bool
nLRFUHeap<TYPE>::ExistNode( int index )const
{
    return ( (index >= 0) && (index < this->num) );
}

//------------------------------------------------------------------------------
/**
    @param elm new element to insert
    @returns the oldest element if there is not space
*/
template<class TYPE>
TYPE
nLRFUHeap<TYPE>::Insert( TYPE & elm )
{
    Node * node = n_new( Node );
    n_assert_if( node )
    {
        node->data = elm;
        node->crf = this->NodeWeight( 0 );
        node->last = (float)nKernelServer::Instance()->GetTimeServer()->GetTime();

        Node * old = this->ReplaceRoot( node );
        if( old )
        {
            TYPE oldData = old->data;
            n_delete( old );

            return oldData;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param index node to remove
    @returns the remove data
*/
template<class TYPE>
TYPE
nLRFUHeap<TYPE>::Remove( int index )
{
    int last = this->num - 1;

    // if index is not the last
    if( index == last )
    {
        // put index node in the last node
        this->SwapNodes( index, last );

        // shift x up
        int parent = this->ParentNode( index );
        bool alreadyShifted = false;

        while( index && ( this->heap[ index ]->crf < this->heap[ parent ]->crf ) )
        {
            alreadyShifted = true;
            this->SwapNodes( index, parent );

            index = parent;
            parent = this->ParentNode( index );
        }

        // shift x down
        bool isNotLeaf = this->ExistNode( this->LeftNode( index ) );
        bool moveDown = !alreadyShifted;

        while( isNotLeaf && moveDown )
        {
            bool moveDown = false;

            // check with left child
            int child = this->LeftNode( index );
            if( this->heap[ index ]->crf > this->heap[ child ]->crf )
            {
                moveDown = true;
            }

            // check with right child
            int childRight = this->RightNode( index );
            if( this->ExistNode( childRight ) )
            {
                if( this->heap[ index ]->crf > this->heap[ childRight ]->crf )
                {
                    if( this->heap[ childRight ]->crf < this->heap[ child ]->crf )
                    {
                        moveDown = true;
                        child = childRight;
                    }
                }
            }

            // move down 
            if( moveDown )
            {
                this->SwapNodes( index, child );
                index = child;
            }

            isNotLeaf = this->ExistNode( this->LeftNode( index ) );
        }
    }

    // remove last node
    TYPE oldData = this->heap[ last ]->data;

    n_delete( this->heap[ last ] );

    --this->num;

    return oldData;
}

//------------------------------------------------------------------------------
/**
    @param node new node to insert in the heap
    @returns old node
    @retval 0 if there is not neccessary to remove an old node
*/
template<class TYPE>
typename nLRFUHeap<TYPE>::Node*
nLRFUHeap<TYPE>::ReplaceRoot( typename nLRFUHeap<TYPE>::Node * node )
{
    Node * oldNode = 0;

    // if there is not space in the heap
    if( this->num >= this->heap.Size() )
    {
        // remove least used node with new node
        oldNode = this->heap[ 0 ];

        this->heap[ 0 ] = node;

        this->Restore( 0 );
    }
    else
    {
        // insert in the last position
        this->heap[ this->num ] = node;

        int index = this->num++;

        // shift up node until rellocate in the right position
        while( index )
        {
            int parent = this->ParentNode( index );

            if( this->heap[ parent ]->crf > this->heap[ index ]->crf )
            {
                this->SwapNodes( parent, index );
                index = parent;
            }
            else
            {
                break;
            }
        }
    }

    return oldNode;
}

//------------------------------------------------------------------------------
/**
    @param node to rellocate in the heap
*/
template<class TYPE>
void
nLRFUHeap<TYPE>::Restore( int index )
{
    float tc = (float)nKernelServer::Instance()->GetTimeServer()->GetTime();

    float indexCRF = this->NodeWeight( tc - this->heap[ index ]->last ) *
        this->heap[ index ]->crf;

    bool isNotLeaf = this->ExistNode( this->LeftNode( index ) );
    bool moveDown = true;

    while( isNotLeaf && moveDown )
    {
        moveDown = false;

        // check with left child
        int child = this->LeftNode( index );
        float childCRF = this->NodeWeight( tc - this->heap[ child ]->last ) * 
            this->heap[ child ]->crf;
        if( indexCRF > childCRF )
        {
            moveDown = true;
        }

        // check with right child
        int childRight = this->RightNode( index );
        if( this->ExistNode( childRight ) )
        {
            float childRightCRF = this->NodeWeight( tc - this->heap[ childRight ]->last ) * 
                this->heap[ childRight ]->crf;
            if( indexCRF > childRightCRF )
            {
                if( childRightCRF < childCRF )
                {
                    moveDown = true;
                    child = childRight;
                    childCRF = childRightCRF;
                }
            }
        }

        // move down 
        if( moveDown )
        {
            this->SwapNodes( index, child );
            index = child;
        }

        isNotLeaf = this->ExistNode( this->LeftNode( index ) );
    }
}

//------------------------------------------------------------------------------
/**
    @param i1 first node
    @param i2 second node
*/
template<class TYPE>
void
nLRFUHeap<TYPE>::SwapNodes( int i1, int i2 )
{
    Node * temp = this->heap[ i1 ];
    this->heap[ i1 ] = this->heap[ i2 ];
    this->heap[ i2 ] = temp;
}

//------------------------------------------------------------------------------
#endif//N_LRFUHEAP_H