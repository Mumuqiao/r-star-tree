/******************************************************
*Readme
*The comments and part of the code have been modified by @mumuqiao
*The purpose is to make it easy for me to read it and use its functions
*if you want an explanation for major functions in Chinese
*我对代码和注释的内容做出了一些改变，但为了与原来的代码和注释风格项匹配，我并没有将注释改为中文。
*但我对一些主要的功能做了一份中文的文档。即RStarTree.docx
*Please go to https://github.com/Mumuqiao/r-star-tree
* or view my blog and contract me https://joewu.top	
*********************************************************/

#include <string>
#include <ctime>

#include <stdio.h>
#include "RStarTree.h"

//#define RANDOM_DATASET
//#define GUTTMAN_DATASET
#define MY_DATASET
#if defined (RANDOM_DATASET)
	typedef RStarTree<int, 2, 32, 64> 			RTree;
#elif defined(GUTTMAN_DATASET)
	typedef RStarTree<std::string, 2, 2, 3> 	RTree;
#elif defined(MY_DATASET)
	typedef RStarTree<std::string, 4, 2, 3> 	RTree;
#endif

typedef RTree::BoundingBox			BoundingBox;


BoundingBox bounds(int x, int y, int w, int h)
{
	BoundingBox bb;
	
	bb.edges[0].first  = x;
	bb.edges[0].second = x + w; 
	 
	bb.edges[1].first  = y;
	bb.edges[1].second = y + h;
	
	return bb;
}

BoundingBox bounds(int x, int y, int w, int h, int m, int n, int p, int q)
{
	BoundingBox bb;

	bb.edges[0].first = x;
	bb.edges[0].second = x + m;

	bb.edges[1].first = y;
	bb.edges[1].second = y + n;

	bb.edges[2].first = w;
	bb.edges[2].second = w + p;

	bb.edges[3].first = h;
	bb.edges[3].second = h + q;

	return bb;
}

//do something when visting
struct Visitor {
	int count;
	bool ContinueVisiting;
	
	Visitor() : count(0), ContinueVisiting(true) {};
	
	void operator()(const RTree::Leaf * const leaf) 
	{
#if defined( RANDOM_DATASET )
		//std::cout << "Visiting " << count << std::endl;
#elif defined( GUTTMAN_DATASET )
		std::cout << "#" << count << ": visited " << leaf->leaf << " with bound " << leaf->bound.ToString() << std::endl;	
#elif defined(MY_DATASET)
		std::cout << "#" << count << ": visited " << leaf->leaf << " with bound " << leaf->bound.ToString() << std::endl;
#else
		#error "Undefined dataset"
#endif
		count++;
	}
};



int main(int argc, char ** argv)
{
	RTree tree;
	Visitor x;
	std::vector<int> aa;
#ifdef GUTTMAN_DATASET
	tree.Insert( "R8" , bounds( 1,5   , 3,2 ));
	//tree.Print("I1");
	
	tree.Insert( "R9", bounds( 6,1   , 2,2 ));
	//tree.Print("I2");
	
	tree.Insert( "R10", bounds( 6,4   , 2,2 ));
	//tree.Print("I3");
	
	tree.Insert( "R11", bounds( 9,0   , 2,14 ));
	//tree.Print("I4");
	
	tree.Insert( "R13", bounds( 13,1  , 1,9 ));
	//tree.Print("I5");
	
	tree.Insert( "R14", bounds( 12,5  , 2,2 ));
	//tree.Print("I6");
	
	tree.Insert( "R15", bounds( 0,16  , 2,2 ));
	//tree.Print("I7");
	
	tree.Insert( "R16", bounds( 3,11  , 6,7 ));
	//tree.Print("I8");
	
	tree.Insert( "R17", bounds( 14,10 , 7,4 ));
	//tree.Print("I9");
	
	tree.Insert( "R18", bounds( 16,8  , 2,9 ));
	//tree.Print("I10");
	
	tree.Insert( "R19", bounds( 17,12 , 3,3 ));
	//tree.Print("I11");
	
	BoundingBox bound = bounds( 5,10, 5,5 );
	
	std::cout << "查询 " << bound.ToString() << std::endl;
	x = tree.Query(RTree::AcceptOverlapping(bound), Visitor());
	std::cout << "遍历了 " << x.count << " 个节点\n" << std::endl;
	std::cout << "对该区域进行删除" << std::endl;
	tree.RemoveBoundedArea(bound);

	std::cout << "查询 " << bound.ToString() << std::endl;
	x = tree.Query(RTree::AcceptOverlapping(bound), Visitor());
	std::cout << "遍历了 " << x.count << " 个节点\n" << std::endl;

	BoundingBox bound2 = bounds(0, 10, 10, 10);
	std::cout << "删除整个区域 " << bound2.ToString() << std::endl;
	tree.RemoveBoundedArea(bound2);

	std::cout << "查询 " << bound.ToString() << std::endl;
	x = tree.Query(RTree::AcceptOverlapping(bound), Visitor());
	std::cout << "遍历了 " << x.count << " 个节点\n" << std::endl;

	Visitor y = tree.Query(RTree::AcceptAny(), Visitor());
	std::cout << "遍历了 " << y.count << " 个节点\n" << std::endl;
	
	
#endif
	
#ifdef MY_DATASET
	tree.Insert("R8", bounds(1, 5, 3, 2, 7, 4, 2, 6));
	tree.Insert("R9", bounds(6, 1, 2, 2, 10, 4, 5, 1));
	tree.Insert("R10", bounds(6, 2, 2, 2, 3, 6, 4, 9));
	tree.Insert("R11", bounds(9, 0, 2, 14, 2, 1, 1, 6));
	tree.Insert("R13", bounds(13, 1, 1, 9, 2, 4, 3, 6));
	tree.Insert("R14", bounds(12, 5, 2, 2, 6, 1, 3, 6));
	tree.Insert("R15", bounds(0, 16, 2, 2, 7, 4, 9, 5));
	tree.Insert("R16", bounds(3, 11, 6, 3, 3, 3, 1, 2));
	tree.Insert("R17", bounds(14, 7, 7, 4, 10, 2, 6, 7));
	tree.Insert("R18", bounds(16, 8, 2, 9, 3, 6, 6, 1));
	tree.Insert("R19", bounds(17, 2, 3, 3, 4, 2, 2, 8));
	BoundingBox bound = bounds(0, 10, 5, 2, 7, 5, 3, 4);
	std::cout << "查询 " << bound.ToString() << std::endl;
	x = tree.Query(RTree::AcceptOverlapping(bound), Visitor());
	std::cout << "遍历了 " << x.count << " 个节点\n" << std::endl;
	std::cout << "对该区域进行删除" << std::endl;
	tree.RemoveBoundedArea(bound);
	std::cout << "查询 " << bound.ToString() << std::endl;
	x = tree.Query(RTree::AcceptOverlapping(bound), Visitor());
	std::cout << "访问了" << x.count << " 个节点" << std::endl;



	std::cout << "查询 " << bound.ToString() << std::endl;
	x = tree.Query(RTree::AcceptOverlapping(bound), Visitor());
	std::cout << "访问了 " << x.count << " 个节点" << std::endl;

	std::wcout << "对所有节点进行查询" << std::endl;
	Visitor y = tree.Query(RTree::AcceptAny(), Visitor());
	std::cout << "访问了 " << y.count << " 个节点." << std::endl;
#endif
#ifdef RANDOM_DATASET
	srand(time(0));

	#define nodes 20000
	
	for (int i = 0; i < nodes/2; i++)
		tree.Insert(i, bounds( rand() % 1000, rand() % 1000, rand() % 10, rand() % 10));
		
	for (int i = 0; i < nodes/2; i++)
		tree.Insert(i, bounds( rand() % 1000, rand() % 1000, rand() % 20, rand() % 20));
		
	BoundingBox bound = bounds( 100,100, 300,400 );
	
	x = tree.Query(RTree::AcceptAny(), Visitor());
	std::cout << "共搜索到了" << x.count <<"个节点" << "树中有" << tree.GetSize() << "个节点 " << std::endl;
	
	
	std::cout << "搜索 " << bound.ToString() << std::endl;
	x = tree.Query(RTree::AcceptEnclosing(bound), Visitor());
	std::cout << "找到了" << x.count << " 个节点(" << tree.GetSize() << " 个节点在树中)" << std::endl;
	
	std::cout << "删除区域 " << bound.ToString() << std::endl;
	tree.RemoveBoundedArea(bound);
	
	std::cout << "搜索区域 " << bound.ToString() << std::endl;
	x = tree.Query(RTree::AcceptEnclosing(bound), Visitor());
	std::cout << "找到了" << x.count << " 个节点. (" << tree.GetSize() << " 个节点在树中)" << std::endl;
	

		
#endif

	
	
	return 0;
}




