// Octree.h: interface for the COctree class.
//
//	C++ class written by Brett Porter
//	Based on code by Dean Clark for Dr. Dobbs Journal, May 1995
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCTREE_H__873EADA1_9816_11D2_9E25_70C404C10000__INCLUDED_)
#define AFX_OCTREE_H__873EADA1_9816_11D2_9E25_70C404C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define	COLOURBITS	8
#define	TREEDEPTH	6

class COctree  
{
public:
	COctree( DWORD nColours );
	virtual ~COctree();
	void	InsertColour( RGBAPIXEL colour );
	void	MakePaletteTable( RGBAPIXEL pTable[], int &index );
	BYTE	QuantizeColour( RGBAPIXEL colour ) const;
private:
	class	COctNode
	{
	public:
		COctNode( int level, BOOL leaf );
		~COctNode();

		void		MakePaletteTable( RGBAPIXEL pTable[], int &index );
		int			Reduce();
		void		SetNextNode( COctNode *pNextNode );
		COctNode	*GetNextNode() const;
		COctNode	*&GetChild( int level );
		BYTE		QuantizeColour( RGBAPIXEL colour ) const;
		void		AddPixel( const BYTE *pColourArray );
		BOOL		IsLeaf() const;
		static int	GetLevel( const BYTE *pColour, UINT depth );
	private:
		int			m_Level;							// Level for this node
		BOOL		m_IsLeaf;							// TRUE if this is a leaf node
		BYTE		m_Index;							// Colour table index
		DWORD		m_nPixels;							// Total pixels that have this colour
		DWORD		m_RedSum, m_GreenSum, m_BlueSum;	// Sum of the colour components
		RGBAPIXEL	m_Colour;							// Colour at this (leaf) node
		COctNode	*m_pChild[COLOURBITS];						// Tree pointers
		COctNode	*m_pNextNode;						// Reducible list pointer
	};
	void		ReduceTree();
	COctNode	*GetReducible();
	COctNode	*CreateOctNode( int level );
	void		MakeReducible( int level, COctNode *pNode );
	void		InsertNodeAt( COctNode *&pNode, RGBAPIXEL colour, UINT depth );

	COctNode	*m_pReduceList[TREEDEPTH];				// List of reducible nodes
	BYTE		m_LeafLevel;
	DWORD		m_TotalLeaves;
	COctNode	*m_pRoot;
	DWORD		m_nMaxColours;
};

inline BOOL	COctree::COctNode::IsLeaf() const
{
	return m_IsLeaf;
}

inline void	COctree::COctNode::AddPixel( const BYTE *pColourArray )
{
	m_nPixels++;
	m_RedSum += pColourArray[RGBA_RED];
	m_GreenSum += pColourArray[RGBA_GREEN];
	m_BlueSum += pColourArray[RGBA_BLUE];
}

inline void	COctree::COctNode::SetNextNode( COctNode *pNextNode )
{
	m_pNextNode = pNextNode;
}

inline COctree::COctNode	*COctree::COctNode::GetNextNode() const
{
	return m_pNextNode;
}

inline int	COctree::COctNode::GetLevel( const BYTE *pColour, UINT depth )
{
	ASSERT( depth <= TREEDEPTH );

	int	r, g, b, level;

	r = ( pColour[RGBA_RED] & ( 1 << depth )) >> depth;
	g = ( pColour[RGBA_GREEN] & ( 1 << depth )) >> depth;
	b = ( pColour[RGBA_BLUE] & ( 1 << depth )) >> depth;

	level = (( r << 2 ) | ( g << 1 ) | b );
	ASSERT( level < COLOURBITS );

	return level;
}

inline BYTE	COctree::COctNode::QuantizeColour( RGBAPIXEL colour ) const
{
	if ( m_IsLeaf )
		return m_Index;
	else
		return m_pChild[GetLevel(( BYTE* )&colour, TREEDEPTH-m_Level )]->QuantizeColour( colour );
}

inline COctree::COctNode	*&COctree::COctNode::GetChild( int level )
{
	return m_pChild[level];
}

inline void	COctree::MakePaletteTable( RGBAPIXEL pTable[], int &index )
{
	m_pRoot->MakePaletteTable( pTable, index );
}

inline void	COctree::ReduceTree()
{
	m_TotalLeaves -= ( GetReducible()->Reduce()-1 );
}

inline void	COctree::MakeReducible( int level, COctNode *pNode )
{
	pNode->SetNextNode( m_pReduceList[level] );
	m_pReduceList[level] = pNode;
}

inline BYTE	COctree::QuantizeColour( RGBAPIXEL colour ) const
{
	ASSERT( m_pRoot != NULL );

	return m_pRoot->QuantizeColour( colour );
}

#endif // !defined(AFX_OCTREE_H__873EADA1_9816_11D2_9E25_70C404C10000__INCLUDED_)
