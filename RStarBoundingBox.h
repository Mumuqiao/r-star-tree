<<<<<<< HEAD
/*
 *  Copyright (c) 2008 Dustin Spicuzza <dustin@virtualroadside.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2.1 of the GNU Lesser General Public
 *  License as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

=======
>>>>>>> 第一次修改

#ifndef RStarBoundingBox_H
#define RStarBoundingBox_H

#include <limits>
#include <utility>
#include <cstddef>
#include <string>
#include <sstream>


template <std::size_t dimensions>
struct RStarBoundingBox {

	// edges[x].first is low value, edges[x].second is high value
	std::pair<int, int> edges[dimensions];
	
	// forces all edges to their extremes so we can stretch() it
	void reset()
	{
		for (std::size_t axis = 0; axis < dimensions; axis++)
		{
			edges[axis].first = std::numeric_limits<int>::max();
			edges[axis].second = std::numeric_limits<int>::min();
		}
	}
	
	// returns a new bounding box that has the maximum boundaries
	static RStarBoundingBox MaximumBounds()
	{
		RStarBoundingBox<dimensions> bound;
		bound.reset();
		return bound;
	}
	
<<<<<<< HEAD

	// fits another box inside of this box, returns true if a stretch occured
=======
	//stretch the box to fix the bb
>>>>>>> 第一次修改
	bool stretch(const RStarBoundingBox<dimensions> &bb)
	{
		bool ret = false;
		
		for (std::size_t axis = 0; axis < dimensions; axis++)
		{
			
			if (edges[axis].first > bb.edges[axis].first)
			{
				edges[axis].first = bb.edges[axis].first;
				ret = true;
			}
		
			if (edges[axis].second < bb.edges[axis].second)
			{
				edges[axis].second = bb.edges[axis].second;
				ret = true;
			}
		}
			
		return ret;
	}
	
<<<<<<< HEAD
	// the sum of all deltas between edges
=======
>>>>>>> 第一次修改
	inline int edgeDeltas() const
	{
		int distance = 0;
		for (std::size_t axis = 0; axis < dimensions; axis++)
			distance += edges[axis].second - edges[axis].first;
			
		return distance;
	}
	
<<<<<<< HEAD
	// calculates the area of a bounding box
=======
>>>>>>> 第一次修改
	inline double area() const
	{
		double area = 1;
		for (std::size_t axis = 0; axis < dimensions; axis++)
			area *= (double)(edges[axis].second - edges[axis].first);
		
		return area;
	}
	
<<<<<<< HEAD
	// this determines if a bounding box is fully contained within this bounding box
	inline bool encloses(const RStarBoundingBox<dimensions>& bb) const
	{
		// if (y1 < x1 || x2 < y2)
=======
	inline bool encloses(const RStarBoundingBox<dimensions>& bb) const
	{
>>>>>>> 第一次修改
		for (std::size_t axis = 0; axis < dimensions; axis++)
			if (bb.edges[axis].first < edges[axis].first || edges[axis].second < bb.edges[axis].second)
				return false;
		
		return true;
	}
	
<<<<<<< HEAD
	// a quicker way to determine if two bounding boxes overlap
	inline bool overlaps(const RStarBoundingBox<dimensions>& bb) const
	{
		// do it this way so theres no equal signs (in case of doubles)
		// if (!(x1 < y2) && !(x2 > y1))
		for (std::size_t axis = 0; axis < dimensions; axis++)
		{		
			if (!(edges[axis].first < bb.edges[axis].second) || !(bb.edges[axis].first < edges[axis].second))
=======
	inline bool overlaps(const RStarBoundingBox<dimensions>& bb) const
	{
		for (std::size_t axis = 0; axis < dimensions; axis++)
		{		
			if (edges[axis].first >= bb.edges[axis].second || bb.edges[axis].first >= edges[axis].second)
>>>>>>> 第一次修改
				return false;
		}

		return true;
	}
	
<<<<<<< HEAD
	// calculates the total overlapping area of two boxes
=======
>>>>>>> 第一次修改
	double overlap(const RStarBoundingBox<dimensions>& bb) const
	{
		double area = 1.0;
		for (std::size_t axis = 0; area && axis < dimensions; axis++)
		{
<<<<<<< HEAD
			// this makes it easier to understand
=======
>>>>>>> 第一次修改
			const int x1 = edges[axis].first;
			const int x2 = edges[axis].second;
			const int y1 = bb.edges[axis].first;
			const int y2 = bb.edges[axis].second;
		
			// left edge outside left edge
			if (x1 < y1)
			{
				// and right edge inside left edge
				if (y1 < x2)
				{
					// right edge outside right edge
					if (y2 < x2)
						area *= (double)( y2 - y1 );
					else
						area *= (double)( x2 - y1 );
						
					continue;
				}
			}
			// right edge inside left edge
			else if (x1 < y2)
			{
				// right edge outside right edge
				if (x2 < y2)
					area *= (double)( x2 - x1 );
				else
					area *= (double)( y2 - x1 );
					
				continue;
			}
			
			// if we get here, there is no overlap
			return 0.0;
		}

		return area;
	}
	
<<<<<<< HEAD
	// sums the total distances from the center of another bounding box
=======
	//The Euclidean distance of the center of two BoundingBox
>>>>>>> 第一次修改
	double distanceFromCenter(const RStarBoundingBox<dimensions>& bb) const
	{
		double distance = 0, t;
		for (std::size_t axis = 0; axis < dimensions; axis++)
		{
			t = ((double)edges[axis].first + (double)edges[axis].second + 
			     (double)bb.edges[axis].first + (double)bb.edges[axis].second)
				 /2.0;
			distance += t*t;
		}
			
		return distance;
	}
	
<<<<<<< HEAD
	// determines if two bounding boxes are identical
=======

>>>>>>> 第一次修改
	bool operator==(const RStarBoundingBox<dimensions>& bb)
	{
		for (std::size_t axis = 0; axis < dimensions; axis++)
			if (edges[axis].first != bb.edges[axis].first || edges[axis].second != bb.edges[axis].second)
				return false;
			
		return true;
	}
	
	
	// very slow, use for debugging only
	std::string ToString() const
	{
		std::stringstream name("");
		name << "[";
		for (std::size_t axis = 0; axis < dimensions; axis++)
		{
			name << "(" << edges[axis].first << "," << edges[axis].second << ")";
			if (axis != dimensions -1)
				name << ",";
		}
		name << "]";
		
		return name.str();
	}
};



template <std::size_t dimensions>
struct RStarBoundedItem {
	typedef RStarBoundingBox<dimensions> BoundingBox;

	BoundingBox bound;
};


<<<<<<< HEAD
=======
/*Functor: The new feature of C++ */

>>>>>>> 第一次修改
/**********************************************************
 * Functor used to iterate over a set and stretch a
 * bounding box
 **********************************************************/

<<<<<<< HEAD
// for_each(items.begin(), items.end(), StretchBoundedItem::BoundingBox(bound));
=======
>>>>>>> 第一次修改
template <typename BoundedItem>
struct StretchBoundingBox : 
	public std::unary_function< const BoundedItem * const, void >
{
	typename BoundedItem::BoundingBox * m_bound;
	explicit StretchBoundingBox(typename BoundedItem::BoundingBox * bound) : m_bound(bound) {}

	void operator() (const BoundedItem * const item)
	{
		m_bound->stretch(item->bound);
	}
};


/**********************************************************
 * R* Tree related functors used for sorting BoundedItems
 *
 * TODO: Take advantage of type traits
 **********************************************************/

template <typename BoundedItem>
struct SortBoundedItemsByFirstEdge : 
	public std::binary_function< const BoundedItem * const, const BoundedItem * const, bool >
{
	const std::size_t m_axis;
	explicit SortBoundedItemsByFirstEdge (const std::size_t axis) : m_axis(axis) {}
	
	bool operator() (const BoundedItem * const bi1, const BoundedItem * const bi2) const 
	{
		return bi1->bound.edges[m_axis].first < bi2->bound.edges[m_axis].first;
	}
};

template <typename BoundedItem>
struct SortBoundedItemsBySecondEdge : 
	public std::binary_function< const BoundedItem * const, const BoundedItem * const, bool >
{
	const std::size_t m_axis;
	explicit SortBoundedItemsBySecondEdge (const std::size_t axis) : m_axis(axis) {}

	bool operator() (const BoundedItem * const bi1, const BoundedItem * const bi2) const 
	{
		return bi1->bound.edges[m_axis].second < bi2->bound.edges[m_axis].second;
	}
};


template <typename BoundedItem>
struct SortBoundedItemsByDistanceFromCenter : 
	public std::binary_function< const BoundedItem * const, const BoundedItem * const, bool >
{
	const typename BoundedItem::BoundingBox * const m_center;
	explicit SortBoundedItemsByDistanceFromCenter(const typename BoundedItem::BoundingBox * const center) : m_center(center) {}

	bool operator() (const BoundedItem * const bi1, const BoundedItem * const bi2) const 
	{
		return bi1->bound.distanceFromCenter(*m_center) < bi2->bound.distanceFromCenter(*m_center);
	}
};

template <typename BoundedItem>
struct SortBoundedItemsByAreaEnlargement : 
	public std::binary_function< const BoundedItem * const, const BoundedItem * const, bool >
{
	const double area;
	explicit SortBoundedItemsByAreaEnlargement(const typename BoundedItem::BoundingBox * center) : area(center->area()) {}

	bool operator() (const BoundedItem * const bi1, const BoundedItem * const bi2) const 
	{
		return area - bi1->bound.area() < area - bi2->bound.area();
	}
};

template <typename BoundedItem>
struct SortBoundedItemsByOverlapEnlargement : 
	public std::binary_function< const BoundedItem * const, const BoundedItem * const, bool >
{
	const typename BoundedItem::BoundingBox * const m_center;
	explicit SortBoundedItemsByOverlapEnlargement(const typename BoundedItem::BoundingBox * const center) : m_center(center) {}

	bool operator() (const BoundedItem * const bi1, const BoundedItem * const bi2) const 
	{
		return bi1->bound.overlap(*m_center) < bi2->bound.overlap(*m_center);
	}
};


#endif
