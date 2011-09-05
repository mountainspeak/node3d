#include "Terrain.h"
#include <fstream>
#include "RenderSystem.h"
#include "Graphics.h"

CTerrain::CTerrain()
	:m_pTerrainData(NULL)
	,m_pVB(NULL)
	,m_pIB(NULL)
	,m_bShowBox(false)
	,m_uShowTileIBCount(0)
{
	m_Tiles[0][0] = "Terrain.0_0";
	m_Tiles[0][1] = "Terrain.0_1";
	m_Tiles[0][2] = "Terrain.0_2";
	m_Tiles[0][3] = "Terrain.0_3";
	m_Tiles[0][4] = "Terrain.0_4";
	m_Tiles[0][5] = "Terrain.0_5";
	m_Tiles[0][6] = "Terrain.0_6";
	m_Tiles[0][7] = "Terrain.0_7";
	m_Tiles[0][8] = "Terrain.0_8";

	m_Tiles[1][0] = "Terrain.1_0";
	m_Tiles[1][1] = "Terrain.1_1";
	m_Tiles[1][2] = "Terrain.1_2";
	m_Tiles[1][3] = "Terrain.1_3";
	m_Tiles[1][4] = "Terrain.1_4";
	m_Tiles[1][5] = "Terrain.1_5";
	m_Tiles[1][6] = "Terrain.1_6";
	m_Tiles[1][7] = "Terrain.1_7";
	m_Tiles[1][8] = "Terrain.1_8";
}

CTerrain::~CTerrain()
{
	S_DEL(m_pVB);
	S_DEL(m_pIB);
}

void CTerrain::setTileMaterial(int nTileID, const std::string& strMaterialName)
{
	m_Tiles[0][nTileID] = strMaterialName;
}

CMaterial& CTerrain::getMaterial(const char* szMaterialName)
{
	return GetRenderSystem().getMaterialMgr().getItem(szMaterialName);
}

void CTerrain::updateIB()
{
	if (m_pTerrainData==NULL)
	{
		return;
	}
	m_RenderTileSubsLayer[0].clear();
	m_RenderTileSubsLayer[1].clear();
	// 写IB
	if (m_pIB)
	{
		for (auto it=m_setRenderChunks.begin(); it!=m_setRenderChunks.end(); ++it)
		{
			for (size_t y = (*it)->box.vMin.z; y<(*it)->box.vMax.z; ++y)
			{
				for (size_t x = (*it)->box.vMin.x; x<(*it)->box.vMax.x; ++x)
				{
					TerrainCell& cell = *m_pTerrainData->getCell(x,y);
					if ((cell.uAttribute&0x8)==0)// 透明
					{
						unsigned long uIndex = y*(m_pTerrainData->getWidth()+1)+x;
						const unsigned char uTileID0 = cell.uTileID[0];
						if (uTileID0!=0xFF)// 透明
						{
							m_RenderTileSubsLayer[0][uTileID0].myVertexIndex(uIndex);
						}
						const unsigned char uTileID1 = cell.uTileID[1];
						if (uTileID1!=0xFF)// 透明
						{
							m_RenderTileSubsLayer[1][uTileID1].myVertexIndex(uIndex);
						}
					}
				}
			}
		}

		m_uShowTileIBCount = 0;
		for (int nLayer=0; nLayer<2; nLayer++)
		{
			for (auto it = m_RenderTileSubsLayer[nLayer].begin(); it!=m_RenderTileSubsLayer[nLayer].end(); it++)
			{
				it->second.istart = m_uShowTileIBCount;
				m_uShowTileIBCount += it->second.icount;
				it->second.icount = 0;
				it->second.vcount=it->second.vcount-it->second.vstart+1+(m_pTerrainData->getWidth()+1)+1;
			}
		}

		unsigned long uTempVertexXCount = m_pTerrainData->getWidth()+1;
		unsigned short* index = (unsigned short*)m_pIB->lock(0, m_uShowTileIBCount*sizeof(unsigned short),CHardwareBuffer::HBL_NO_OVERWRITE);
		for (auto it=m_setRenderChunks.begin(); it!=m_setRenderChunks.end(); it++)
		{
			for (size_t y = (*it)->box.vMin.z; y<(*it)->box.vMax.z; ++y)
			{
				for (size_t x = (*it)->box.vMin.x; x<(*it)->box.vMax.x; ++x)
				{
					TerrainCell& cell = *m_pTerrainData->getCell(x,y);
					if ((cell.uAttribute&0x8)==0)// 透明
					{
						unsigned long uIndex = m_pTerrainData->getVertexIndex(x,y);
						for (int nLayer=0; nLayer<2; nLayer++)
						{
							const unsigned char uTileID = cell.uTileID[nLayer];
							TerrainSub& sub = m_RenderTileSubsLayer[nLayer][uTileID];
							if (uTileID!=0xFF)
							{
								// 2	 3
								//	*---*
								//	| / |
								//	*---*
								// 0	 1
								unsigned short* p = index+sub.istart+sub.icount;
								sub.icount+=6;
								*p = uIndex;
								p++;
								*p = uIndex+uTempVertexXCount;
								p++;
								*p = uIndex+uTempVertexXCount+1;
								p++;

								*p = uIndex;
								p++;
								*p = uIndex+uTempVertexXCount+1;
								p++;
								*p = uIndex+1;
								p++;
							}
						}
					}
				}
			}
		}
		m_pIB->unlock();
	}
}

void CTerrain::updateVB(int nBeginX, int nBeginY, int nEndX, int nEndY)
{
	if (m_pVB==NULL || m_pTerrainData==NULL)
	{
		return;
	}
	unsigned long uOffset	= sizeof(TerrainVertex)*m_pTerrainData->getVertexIndex(nBeginX,nBeginY);
	unsigned long uSize	= sizeof(TerrainVertex)*(m_pTerrainData->getVertexIndex(nEndX,nEndY)+1)-uOffset;
	if (uSize>0)
	{
		TerrainVertex*	pV = (TerrainVertex*)m_pVB->lock(uOffset, uSize, CHardwareBuffer::HBL_NO_OVERWRITE/*CHardwareBuffer::HBL_NORMAL*/);
		for (int y = nBeginY; y <= nEndY; ++y)
		{
			for (int x = nBeginX; x <= nEndX; ++x)
			{
				m_pTerrainData->getVertexByCell(x,y,*pV);
				pV++;
			}
		}
		m_pVB->unlock();
	}
}

bool CTerrain::init(void* pData)
{
	m_pTerrainData = (CTerrainData*)pData;
	m_RenderTileSubsLayer[0].clear();
	m_RenderTileSubsLayer[1].clear();
	m_setRenderChunks.clear();

	S_DEL(m_pVB);
	m_pVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(m_pTerrainData->getVertexCount(),sizeof(TerrainVertex));
	updateVB(0,0,m_pTerrainData->getWidth(),m_pTerrainData->getHeight());/*CHardwareBuffer::HBL_NORMAL*/

	S_DEL(m_pIB);
	m_pIB = GetRenderSystem().GetHardwareBufferMgr().CreateIndexBuffer(m_pTerrainData->getWidth()*m_pTerrainData->getHeight()*2*6*sizeof(unsigned short),CHardwareIndexBuffer::IT_16BIT,CHardwareBuffer::HBU_WRITE_ONLY);

	m_LightMapDecal.createBySize((m_pTerrainData->getWidth()+1), m_pTerrainData->getChunkSize(), m_pTerrainData->getChunkSize());
	m_LightDecal.createBySize((m_pTerrainData->getWidth()+1),8,8);
	return true;
}

bool CTerrain::prepare()const
{
	if (m_pVB==NULL||m_pIB==NULL)
	{
		return false;
	}
	CRenderSystem& R = GetRenderSystem();
	R.SetLightingEnabled(false);
	R.SetCullingMode(CULL_ANTI_CLOCK_WISE);
	R.SetDepthBufferFunc(true,true);

	R.SetFVF(TERRAIN_VERTEX_FVF);
	R.SetStreamSource(0, m_pVB, 0, sizeof(TerrainVertex));
	R.SetIndices(m_pIB);
	return true;
}

void CTerrain::updateRender(const CFrustum& frustum)
{
	m_setRenderChunks.clear();
	if (m_pTerrainData)
	{
		m_pTerrainData->walkOctree(frustum,m_setRenderChunks);
	}
	//
	updateIB();
}

void CTerrain::drawCubeBoxes(Color32 color)const
{
	CRenderSystem& R = GetRenderSystem();
	R.SetDepthBufferFunc(true,true);
	//for (size_t i=0; i<m_setRenderChunks.size(); ++i)
	{
	//	GetGraphics().drawBBox(m_setRenderChunks[i]->bbox,color);
	}
}

void CTerrain::drawLayer(int nLayer)const
{
	CRenderSystem& R = GetRenderSystem();
	R.SetTexCoordIndex(0, 0);	// Diffuse
	R.SetTexCoordIndex(1, 1);	// lightmap
	for (auto it = m_RenderTileSubsLayer[nLayer].begin(); it!=m_RenderTileSubsLayer[nLayer].end(); ++it)
	{
		auto itTile = m_Tiles[nLayer].find(it->first);
		if (itTile!=m_Tiles[nLayer].end() && R.prepareMaterial(itTile->second.c_str()))
		{
			R.drawIndexedSubset(it->second);
			R.finishMaterial();
		}
	}
}

void CTerrain::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	if (eRenderType&MATERIAL_GEOMETRY)
	{
		if (m_bShowBox)
		{
			drawCubeBoxes();
		}
		if (prepare())
		{
			drawLayer(0);
			drawLayer(1);
		}
	}
}

void CTerrain::draw()
{
	if (m_LightMapDecal.setIB())
	{
		for (auto it=m_setRenderChunks.begin(); it!=m_setRenderChunks.end(); ++it)
		{
			drawChunk(*it);
		}
	}
}

void CTerrain::drawChunk(TerrainChunk* pChunk)
{
	unsigned long uBaseVertexIndex = m_pTerrainData->getVertexIndex(pChunk->box.vMin.x, pChunk->box.vMin.y);
	m_LightMapDecal.Draw(uBaseVertexIndex);
}

void CTerrain::brushLightColor(MAP_EDIT_RECORD& mapEditRecord, float fPosX, float fPosY, Color32 colorPaint, float fRadius, float fHardness, float fStrength)
{
	/*if (isPointIn(fPosX, fPosY))
	{
		EditTarget editTarget;
		//editTarget.type = CTerrainBrush::BRUSH_TYPE_TERRAIN_COLOR;
		for (int y=fPosY-fRadius; y<fPosY+fRadius; y++)
		{
			for (int x=fPosX-fRadius; x<fPosX+fRadius; x++)
			{
				editTarget.x = x;
				editTarget.y = y;
				if (isPointIn(x,y))
				{
					Vec2D vLength(fPosX-x, fPosY-y);
					float fOffset = 1.0f-vLength.length()/fRadius;
					fOffset = min(fOffset/(1.0f-fHardness),1.0f);
					if (fOffset>0)
					{
						float fRate=fOffset*fStrength;
						Color32& colorDest = (Color32)mapEditRecord[editTarget].color;
						if(colorDest.c==0)
						{
							colorDest=getVertexColor(x,y);
						}
						colorDest += colorPaint*fRate;
					}
				}
			}
		}
	}*/
}

void CTerrain::drawLightDecal(float x, float y, float fSize, Color32 color)
{
	if (!m_LightDecal.setIB())
	{
		return;
	}
	CRenderSystem& R = GetRenderSystem();
	R.SetSamplerAddressUV(0,ADDRESS_BORDER,ADDRESS_BORDER);
	// 打开纹理矩阵

	float fScale = 0.5f/fSize;
	// －贴花中心Ｕ×缩放比＋０.５纹理偏移
	float fPosX = 0.5f-x*fScale;
	// １－（－贴花中心Ｖ×缩放比＋０.５纹理偏移）
	float fPosY = 0.5f+y*fScale;
	//
	Matrix mLightDecalTrans = Matrix(
		fScale,0,fPosX,0,
		0,-fScale,fPosY,0,
		0,0,0,0,
		0,0,0,0);
	R.setTextureMatrix(0,TTF_COUNT2,mLightDecalTrans);
	m_LightDecal.Draw((int)x,(int)y);

	//
	R.setTextureMatrix(0,TTF_DISABLE);
	R.SetSamplerAddressUV(0,ADDRESS_WRAP,ADDRESS_WRAP);
}