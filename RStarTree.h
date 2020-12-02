

#ifndef RSTARTREE_H
#define RSTARTREE_H

#include <list>
#include <vector>
#include <limits>
#include <algorithm>
#include <cassert>
#include <functional>

#include <iostream>
#include <sstream>
#include <fstream>

#include "RStarBoundingBox.h"

// R* tree parameters
#define RTREE_REINSERT_P 0.30
#define RTREE_CHOOSE_SUBTREE_P 32

// template definition:
#define RSTAR_TEMPLATE 


// definition of an leaf
template <typename BoundedItem, typename LeafType>
struct RStarLeaf : BoundedItem {
	
	typedef LeafType leaf_type;
	LeafType leaf;
};

// definition of a node
template <typename BoundedItem>
struct RStarNode : BoundedItem {
	std::vector< BoundedItem* > items;
	bool hasLeaves;
};

#include "RStarVisitor.h"


/**
	\class RStarTree
	\brief Implementation of an RTree with an R* index
	
	@tparam LeafType		type of leaves stored in the tree
	@tparam dimensions  	number of dimensions the bounding boxes are described in  Î¬Êý
	@tparam	min_child_items m, in the range 2 <= m < M
	@tparam max_child_items M, in the range 2 <= m < M
	@tparam	RemoveLeaf 		A functor used to remove leaves from the tree
*/
template <
	typename LeafType, 
	std::size_t dimensions, std::size_t min_child_items, std::size_t max_child_items
>
class RStarTree {
public:

	// shortcuts
	typedef RStarBoundedItem<dimensions>		BoundedItem;
	typedef typename BoundedItem::BoundingBox	BoundingBox;
	
	typedef RStarNode<BoundedItem> 				Node;
	typedef RStarLeaf<BoundedItem, LeafType> 	Leaf;
	
	// acceptors
	typedef RStarAcceptOverlapping<Node, Leaf>	AcceptOverlapping;
	typedef RStarAcceptEnclosing<Node, Leaf>	AcceptEnclosing;
	typedef RStarAcceptAny<Node, Leaf>			AcceptAny;

	// predefined visitors
	typedef RStarRemoveLeaf<Leaf>				RemoveLeaf;
	typedef RStarRemoveSpecificLeaf<Leaf>		RemoveSpecificLeaf;
	

	// default constructor
	RStarTree() : m_root(NULL), m_size(0) 
	{
		//assert(1 <= min_child_items && min_child_items <= max_child_items/2);
	}
	
	// destructor
	~RStarTree() { 
		Remove(
			AcceptAny(), 
			RemoveLeaf()
		);
	}
	
	// Single insert function, adds a new item to the tree
	void Insert(LeafType leaf, const BoundingBox &bound)
	{
		Leaf * newLeaf = new Leaf();
		newLeaf->bound = bound;
		newLeaf->leaf  = leaf;

		// create a new root node if necessary
		if (!m_root)
		{
			m_root = new Node();
			m_root->hasLeaves = true;
			
			// reserve memory
			m_root->items.reserve(min_child_items);
			m_root->items.push_back(newLeaf);
			m_root->bound = bound;
		}
		else
			// start the insertion process
			InsertInternal(newLeaf, m_root);
			
		m_size += 1;
	}

	
	/**	
		@param acceptor 		An acceptor functor that returns true if this 
		branch or leaf of the tree should be considered for visitation.
		
		@param visitor			A visitor functor that does the visiting
		
		@return This will return the Visitor object, so you can retrieve whatever
		data it has in it if needed (for example, to get the count of items
		visited). It returns by value, so ensure that the copy is cheap
		for decent performance.
	*/
	template <typename Acceptor, typename Visitor>    
	Visitor Query(const Acceptor &accept, Visitor visitor)             
	{
		if (m_root)
		{	
			QueryFunctor<Acceptor, Visitor> query(accept, visitor);
			query(m_root);
		}
		
		return visitor;
	}

	
	/**
		
		@param acceptor 	A node acceptor functor that returns true if this 
		branch or leaf of the tree should be considered for deletion 
		(it does not delete it, however. That is what the LeafRemover does).

		@param leafRemover		A visitor functor that decides whether that 
		individual item should be removed from the tree. If it returns true, 
		then the node holding that item will be deleted.
	*/
	//Has called in RemoveBoundedArea and Removeitem      
	template <typename Acceptor, typename LeafRemover>
	void Remove( const Acceptor &accept, LeafRemover leafRemover)
	{
		std::list<Leaf*> itemsToReinsert;

		if (!m_root)
			return;
		
		RemoveFunctor<Acceptor, LeafRemover> remove(accept, leafRemover, &itemsToReinsert, &m_size);   
		remove(m_root, true);
		
		if (!itemsToReinsert.empty())
		{
			typename std::list< Leaf* >::iterator it = itemsToReinsert.begin();
			typename std::list< Leaf* >::iterator end = itemsToReinsert.end();
			
			for(;it != end; it++)
				InsertInternal(*it, m_root);
		}
	}
	
	// stub that removes any items contained in an specified area
	void RemoveBoundedArea( const BoundingBox &bound )
	{
		Remove(AcceptEnclosing(bound), RemoveLeaf());
	}
	
	// removes a specific item. If removeDuplicates is true, only the first
	// item found will be removed
	void RemoveItem( const LeafType &item, bool removeDuplicates = true )
	{
		Remove( AcceptAny(), RemoveSpecificLeaf(item, removeDuplicates));
	}
	
	
	std::size_t GetSize() const { return m_size; }
	std::size_t GetDimensions() const { return dimensions; }
	
	
protected:
	
	//Has changed 
	Node * ChooseSubtree(Node * node, const BoundingBox * bound)  
	{
		if (static_cast<Node*>(node->items[0])->hasLeaves)
		{
			// determine the minimum overlap cost
			if (max_child_items > (RTREE_CHOOSE_SUBTREE_P*2)/3  && node->items.size() > RTREE_CHOOSE_SUBTREE_P)
			{
			//if the count of items is big enough,then choose 1/3 of them to reinsert
				
				// Let A be the group of the first p entrles
				std::partial_sort( node->items.begin(), node->items.begin() + RTREE_CHOOSE_SUBTREE_P, node->items.end(), 
					SortBoundedItemsByAreaEnlargement<BoundedItem>(bound));
				
				// From the items in A, considering all items in
				// N, choose the leaf whose rectangle needs least
				// overlap enlargement
				
				return static_cast<Node*>(* std::min_element(node->items.begin(), node->items.begin() + RTREE_CHOOSE_SUBTREE_P,
					SortBoundedItemsByOverlapEnlargement<BoundedItem>(bound)));
			}

			// choose the leaf in N whose rectangle needs least
			// overlap enlargement to include the new data
			// rectangle Resolve ties by choosmg the leaf
			// whose rectangle needs least area enlargement, then
			// the leaf with the rectangle of smallest area
			
			return static_cast<Node*>(* std::min_element(node->items.begin(), node->items.end(),
				SortBoundedItemsByOverlapEnlargement<BoundedItem>(bound)));	
		}
		
		// if the chlld pointers in N do not point to leaves

		// [determine the minimum area cost],
		// choose the leaf in N whose rectangle needs least
		// area enlargement to include the new data
		// rectangle. Resolve ties by choosing the leaf
		// with the rectangle of smallest area
			
		return static_cast<Node*>(*	std::min_element( node->items.begin(), node->items.end(),
				SortBoundedItemsByAreaEnlargement<BoundedItem>(bound)));
	}
	
	
	//Its recursive calls are very confusing, and I'm trying to figure out a way to optimize it

	Node * InsertInternal(Leaf * leaf, Node * node, bool firstInsert = true)
	{
		//adjust the node
		node->bound.stretch(leaf->bound);
	
	
		if (node->hasLeaves)
		{
			//add the leaf
			node->items.push_back(leaf);
		}
		else
		{
			//Recursive call the InsertInternal
			Node * tmp_node = InsertInternal( leaf, ChooseSubtree(node, &leaf->bound), firstInsert );
			
			if (!tmp_node)
				return NULL;
				
			// this gets joined to the list of items at this level
			node->items.push_back(tmp_node);
		}
		
		
		// If N has M+1 items. invoke OverflowTreatment with the
		// level of N as a parameter [for reinsertion or split]
		if (node->items.size() > max_child_items )
		{
			//return the overflow nodes.
			return OverflowTreatment(node, firstInsert);
		}
			
		return NULL;
	}


	//Select a path with minimal overlap
	Node * OverflowTreatment(Node * level, bool firstInsert)
	{
		//if the node is the first time that has been reinsert
		//to optimize the structure of tree
		if (level != m_root && firstInsert)
		{
			Reinsert(level);
			return NULL;
		}
		
		Node * splitItem = Split(level);
		
		// If OverflowTreatment caused a split of the root, create a new root
		if (level == m_root)
		{
			Node * newRoot = new Node();
			newRoot->hasLeaves = false;
			
			// reserve memory
			newRoot->items.reserve(min_child_items);
			newRoot->items.push_back(m_root);
			newRoot->items.push_back(splitItem);
			
			newRoot->bound.reset();
			for_each(newRoot->items.begin(), newRoot->items.end(), StretchBoundingBox<BoundedItem>(&newRoot->bound));
			
			m_root = newRoot;
			return NULL;
		}

		return splitItem;
	}
	
	// This returns a node, which should be added to the items of the
	// passed node's parent
	Node * Split(Node * node)
	{
		Node * newNode = new Node();
		newNode->hasLeaves = node->hasLeaves;

		const std::size_t n_items = node->items.size();
		const std::size_t distribution_count = n_items - 2*min_child_items + 1;
		
		std::size_t split_axis = dimensions+1, split_edge = 0, split_index = 0;
		int split_margin = 0;
		
		BoundingBox R1, R2;

		// these should always hold true
		assert(n_items == max_child_items + 1);
		assert(distribution_count > 0);
		assert(min_child_items + distribution_count-1 <= n_items);
		

		//ChooseSplitAxis  return for  split_index
		for (std::size_t axis = 0; axis < dimensions; axis++)
		{
			// initialize per-loop items
			int margin = 0;
			double overlap = 0, dist_area, dist_overlap;
			std::size_t dist_edge = 0, dist_index = 0;
		
			dist_area = dist_overlap = std::numeric_limits<double>::max();		
		
			for (std::size_t edge = 0; edge < 2; edge++)
			{
				if (edge == 0)
					std::sort(node->items.begin(), node->items.end(), SortBoundedItemsByFirstEdge<BoundedItem>(axis));
				else
					std::sort(node->items.begin(), node->items.end(), SortBoundedItemsBySecondEdge<BoundedItem>(axis));
				for (std::size_t k = 0; k < distribution_count; k++)
		        {
					double area = 0;
				
					// calculate bounding box of R1
					R1.reset();
					for_each(node->items.begin(), node->items.begin()+(min_child_items+k), StretchBoundingBox<BoundedItem>(&R1));
							
					// then do the same for R2
					R2.reset();
					for_each(node->items.begin()+(min_child_items+k+1), node->items.end(), StretchBoundingBox<BoundedItem>(&R2));
					

					margin 	+= R1.edgeDeltas() + R2.edgeDeltas();
					area 	+= R1.area() + R2.area();		//Caculate the area so that we can choose a best split index, but it will cost some time
					overlap =  R1.overlap(R2);
					
					//updata the best split index
					if (overlap < dist_overlap || (overlap == dist_overlap && area < dist_area))
					{
						dist_edge = 	edge;
						dist_index = 	min_child_items+k;
						dist_overlap = 	overlap;
						dist_area = 	area;
					}		
				}
			}
			
			if (split_axis == dimensions+1 || split_margin > margin )
			{
				split_axis 		= axis;
				split_margin 	= margin;
				split_edge 	= dist_edge;
				split_index 	= dist_index;
			}
		}
		
		//ChooseSplitIndex change the source node and return the new node  
		if (split_edge == 0)
			std::sort(node->items.begin(), node->items.end(), SortBoundedItemsByFirstEdge<BoundedItem>(split_axis));

		// only reinsert the sort key if we have to
		else if (split_axis != dimensions-1)
			std::sort(node->items.begin(), node->items.end(), SortBoundedItemsBySecondEdge<BoundedItem>(split_axis));	
		
		// distribute the end of the array to the new node, then erase them from the original node
		newNode->items.assign(node->items.begin() + split_index, node->items.end());
		node->items.erase(node->items.begin() + split_index, node->items.end());
		
		// adjust the bounding box for each 'new' node
		node->bound.reset();
		std::for_each(node->items.begin(), node->items.end(), StretchBoundingBox<BoundedItem>(&node->bound));
		
		newNode->bound.reset();
		std::for_each(newNode->items.begin(), newNode->items.end(), StretchBoundingBox<BoundedItem>(&newNode->bound));
		
		return newNode;
	}
	
	// This routine is used to do the opportunistic reinsertion that the
	// R* algorithm calls for
	
	void Reinsert(Node * node)
	{
		std::vector< BoundedItem* > removed_items;

		const std::size_t n_items = node->items.size();
		const std::size_t p = (std::size_t)((double)n_items * RTREE_REINSERT_P) > 0 ? (std::size_t)((double)n_items * RTREE_REINSERT_P) : 1;
		
		// RI1 For all M+l items of a node N, compute the distance
		// between the centers of their rectangles and the center
		// of the bounding rectangle of N
		assert(n_items == max_child_items + 1);
		
		std::partial_sort(node->items.begin(), node->items.end() - p, node->items.end(), 
			SortBoundedItemsByDistanceFromCenter<BoundedItem>(&node->bound));
			
		// Remove the last p items from N
		removed_items.assign(node->items.end() - p, node->items.end());
		node->items.erase(node->items.end() - p, node->items.end());
		
		// adjust the bounding rectangle of N
		node->bound.reset();
		for_each(node->items.begin(), node->items.end(), StretchBoundingBox<BoundedItem>(&node->bound));
		
		//the firstinser is flase!
		for (typename std::vector< BoundedItem* >::iterator it = removed_items.begin(); it != removed_items.end(); it++)
			InsertInternal( static_cast<Leaf*>(*it), m_root, false);
	}
	
	/****************************************************************
	 * These are used to implement walking the entire R* tree in a
	 * conditional way
	 ****************************************************************/

	// visits a node if necessary
	template <typename Acceptor, typename Visitor>
	struct VisitFunctor : std::unary_function< const BoundingBox *, void > {
	
		const Acceptor &accept;
		Visitor &visit;
		
		explicit VisitFunctor(const Acceptor &a, Visitor &v) : accept(a), visit(v) {}
	
		void operator()( BoundedItem * item ) 
		{
			Leaf * leaf = static_cast<Leaf*>(item);
		
			if (accept(leaf))
				visit(leaf);
		}
	};
	
	
	// this functor recursively walks the tree
	template <typename Acceptor, typename Visitor>
	struct QueryFunctor : std::unary_function< const BoundedItem, void > {
		const Acceptor &accept;
		Visitor &visitor;
		
		explicit QueryFunctor(const Acceptor &a, Visitor &v) : accept(a), visitor(v) {}
	
		void operator()(BoundedItem * item)
		{
			Node * node = static_cast<Node*>(item);
		
			if (visitor.ContinueVisiting && accept(node))
			{
				if (node->hasLeaves)
					for_each(node->items.begin(), node->items.end(), VisitFunctor<Acceptor, Visitor>(accept, visitor));
				else
					for_each(node->items.begin(), node->items.end(), *this);
			}
		}
	};
	
	
	/****************************************************************
	 * Used to remove items from the tree
	 *
	 * At some point, the complexity just gets ridiculous. I'm pretty
	 * sure that the remove functions are close to that by now... 
	 ****************************************************************/
	

	
	// determines whether a leaf should be deleted or not
	template <typename Acceptor, typename LeafRemover>
	struct RemoveLeafFunctor : 
		std::unary_function< const BoundingBox *, bool > 
	{
		const Acceptor &accept;
		LeafRemover &remove;
		std::size_t * size;
		
		explicit RemoveLeafFunctor(const Acceptor &a, LeafRemover &r, std::size_t * s) :
			accept(a), remove(r), size(s) {}
	
		bool operator()(BoundedItem * item ) const {
			Leaf * leaf = static_cast<Leaf *>(item);
			
			if (accept(leaf) && remove(leaf))
			{
				--(*size);
				delete leaf;
				return true;
			}
			
			return false;
		}
	};
	
	
	template <typename Acceptor, typename LeafRemover>
	struct RemoveFunctor :
		std::unary_function< const BoundedItem *, bool > 
	{
		const Acceptor &accept;
		LeafRemover &remove;
		
		// parameters that are passed in
		std::list<Leaf*> * itemsToReinsert;
		std::size_t * m_size;
	
		// the third parameter is a list that the items that need to be reinserted
		// are put into
		explicit RemoveFunctor(const Acceptor &na, LeafRemover &lr, std::list<Leaf*>* ir, std::size_t * size)
			: accept(na), remove(lr), itemsToReinsert(ir), m_size(size) {}
	
		bool operator()(BoundedItem * item, bool isRoot = false)
		{
			Node * node = static_cast<Node*>(item);
		
			if (accept(node))
			{	
				// this is the easy part: remove nodes if they need to be removed
				if (node->hasLeaves)
					node->items.erase(std::remove_if(node->items.begin(), node->items.end(), RemoveLeafFunctor<Acceptor, LeafRemover>(accept, remove, m_size)), node->items.end());
				else
					node->items.erase(std::remove_if(node->items.begin(), node->items.end(), *this), node->items.end() );

				if (!isRoot)
				{
					if (node->items.empty())
					{
						// tell parent to remove us if theres nothing left
						delete node;
						return true;
					}
					else if (node->items.size() < min_child_items)
					{
						// queue up the items that need to be reinserted
						QueueItemsToReinsert(node);
						return true;
					}
				}
				else if (node->items.empty())
				{
					// if the root node is empty, setting these won't hurt
					// anything, since the algorithms don't actually require 
					// the nodes to have anything in them. 
					node->hasLeaves = true;
					node->bound.reset();
				}
			}			
			
			// anything else, don't remove it
			return false;
			
		}
		
		// theres probably a better way to do this, but this
		// traverses and finds any leaves, and adds them to a
		// list of items that will later be reinserted
		void QueueItemsToReinsert(Node * node)
		{
			typename std::vector< BoundedItem* >::iterator it = node->items.begin();
			typename std::vector< BoundedItem* >::iterator end = node->items.end();
		
			if (node->hasLeaves)
			{
				for(; it != end; it++)
					itemsToReinsert->push_back(static_cast<Leaf*>(*it));
			}
			else
				for (; it != end; it++)
					QueueItemsToReinsert(static_cast<Node*>(*it));
					
			delete node;
		}
	};
	

private:
	Node * m_root;
	
	std::size_t m_size;
};

#undef RSTAR_TEMPLATE

#undef RTREE_SPLIT_M
#undef RTREE_REINSERT_P
#undef RTREE_CHOOSE_SUBTREE_P




#endif

