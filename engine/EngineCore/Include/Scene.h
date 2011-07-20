#pragma once
#include "Terrain.h"
#include "MapObj.h"
#include "3DMapSceneObj.h"
#include "3DMapEffect.h"
#include "Octree.h"
#include "FocusNode.h"

class SceneData: public iSceneData
{
public:
	virtual GSET_CONST_VAR	(BBox&,				m_,BBox);
	virtual GSET_CONST_VAR	(size_t,			m_,OctreeDepth);
	virtual GSET_CONST_VAR	(Fog&,				m_,Fog);
	virtual GSET_CONST_VAR	(DirectionalLight&,	m_,Light);
private:
	BBox				m_BBox;
	size_t				m_OctreeDepth;
	Fog					m_Fog;
	DirectionalLight	m_Light;
};

class CScene: public CRenderNode
{
public:
	CScene();
	~CScene();
public:
	virtual int			getType					(){return NODE_BASE;}
	virtual void		getRenderNodes			(const CFrustum& frustum, LIST_RENDER_NODE& NodeList);
	void				UpdateRender			(const CFrustum& frustum);
	bool				updateNode				(iRenderNode* pNode);
	// ----
	virtual void		frameMove				(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void		render					(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	// ----
	virtual void		addChild				(iRenderNode* pChild);
	virtual bool		removeChild				(iRenderNode* pChild);
	virtual void		clearChildren			();
	// ----
	// load
	virtual bool		init					(void* pData);
	bool				removeRenderNode		(iRenderNode* pNode);
	// ----
	// # shit
	// ----
	C3DMapEffect*		add3DMapEffect			(const Vec3D& vWorldPos, char* pszIndex, bool bDelself = true);
	void				del3DMapEffect			(const Vec3D& vWorldPos);
	void				del3DMapEffect			(C3DMapEffect* pEffect);
	// ----
	// # shit
	// ----
	virtual	void		clearNodes				();
	virtual	void		getNodes				();
	// ----
	// # Focus Nodes
	// ----
	bool				delChildByFocus			();
	void				updateOctreeByFocus		();
	CFocusNode&			getFocusNodes			(){return m_FocusNode;}
	// ----
	CMapObj*			pickNode				(const Vec3D& vRayPos , const Vec3D& vRayDir);
	// ----
	// # data
	// ----
	Octree<iRenderNode>&getOctree				(){return m_OctreeRoot;}
	// ----
	iTerrainData*		getTerrainData			(){return m_pTerrain;}
	const iTerrainData*	getTerrainData			()const{return m_pTerrain;}
	void				CalcLightMap			();
	// ----
	GSET_VAR		(bool,				m_b,ShowNode);
	GSET_VAR		(bool,				m_b,ShowNodeBBox);
	GSET_VAR		(bool,				m_b,ShowOctreeBox);
	GSET_VAR		(bool,				m_b,RefreshViewport);
	GSET_VAR		(CTerrain*,			m_p,Terrain );
	GSET_CONST_VAR	(DirectionalLight&,	m_,Light);
	GSET_CONST_VAR	(Fog&,				m_,Fog);
	GSET_CONST_VAR	(Vec3D&,			m_v,TargetPos);
	// ----
	SceneData*				getSceneData(){return m_pSceneData;}
protected:
	SceneData*				m_pSceneData;
	CTerrain*				m_pTerrain;
	Octree<iRenderNode>		m_OctreeRoot;
	LIST_RENDER_NODE		m_setRenderSceneNode;
	LIST_RENDER_NODE		m_setLightObj;
	bool					m_bRefreshViewport;
protected:
	CFocusNode				m_FocusNode;        // The node which has focus
	bool					m_bShowNode;
	bool					m_bShowAnimNode;
	bool					m_bShowNodeBBox;
	bool					m_bShowOctreeBox;

	Fog						m_Fog;
	DirectionalLight		m_Light;
	Vec3D					m_vTargetPos;
};