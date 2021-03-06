// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/TerrainNode.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTree.hpp>
#include <Nazara/DynaTerrain/DynaTerrain.hpp>
#include <Nazara/DynaTerrain/Functions.hpp>
#include <stack>
#include <iostream>
#include <Nazara/DynaTerrain/Debug.hpp>


int NzTerrainNode::nbNodes = 0;

NzTerrainNode::NzTerrainNode()
{
    nbNodes++;
    m_isInitialized = false;
}

NzTerrainNode::~NzTerrainNode()
{
    nbNodes--;
}

void NzTerrainNode::CreatePatch()
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        NazaraError("Calling uninitialized node");
        return;
    }
    #endif

    m_patch = NzDynaTerrain::GetTerrainPatch();
    m_patch->Initialize(m_nodeID,m_data);
    m_aabb = m_patch->GetAABB();
    m_patch->UploadMesh();
}

void NzTerrainNode::DeletePatch()
{
    m_patch->UnUploadMesh();
    m_patch->Invalidate();
    NzDynaTerrain::ReturnTerrainPatch(m_patch);
}

const NzBoundingVolumef& NzTerrainNode::GetAABB() const
{
    return m_aabb;
}

NzTerrainNode* NzTerrainNode::GetChild(nzNodeLocation location)
{
    return m_children[location];
}

NzTerrainNode* NzTerrainNode::GetChild(unsigned int i)
{
    if(i < 4)
        return m_children[i];
    else
        return nullptr;
}

NzTerrainNode* NzTerrainNode::GetDirectNeighbor(nzNeighbourDirection direction)
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        NazaraError("Calling uninitialized node");
        return nullptr;
    }
    #endif

    NzTerrainNodeID tempID = m_nodeID;
    int counter = 0;

    switch(direction)
    {
        case nzNeighbourDirection_top :
            tempID.locy -= 1;
        break;

        case nzNeighbourDirection_right :
            tempID.locx += 1;
        break;

        case nzNeighbourDirection_bottom :
            tempID.locy += 1;
        break;

        case nzNeighbourDirection_left :
            tempID.locx -= 1;
        break;
    }

    NzTerrainNode* neighbor;

    if(tempID.IsValid())
    {
        return m_data->quadtree->GetNode(tempID);
    }
    else
    {
        NzTerrainQuadTree* tempQuad = m_data->quadtree->GetNeighbourQuadTree(direction);

        if(tempQuad == nullptr)
            return nullptr;

        //Il y a trois types de connexions :
            // - directe : Top <-> Bottom
            // - orthogonale : Top <-> Left
            // - inverse : Top <-> Top
        nzConnectionType type = m_data->quadtree->GetConnectionType(tempQuad);
        //TODO : Add default
        switch(type)
        {
            case nzConnectionType_orthogonal :
                tempID = m_nodeID;
                tempID.InvertXY();//Pareil, mais dans le cas d'une connexion à 90°
                break;

            case nzConnectionType_straight :
                tempID.Normalize();//Pour passer des coordonnées du bord d'un quadtree au bord de l'autre quadtree
                break;

            case nzConnectionType_reverse :
                tempID = m_nodeID;
                break;
        }

        return tempQuad->GetNode(tempID);
    }
}

int NzTerrainNode::GetNodeAmount()
{
    return nbNodes;
}

const NzTerrainNodeID& NzTerrainNode::GetNodeID() const
{
    return m_nodeID;
}

NzTerrainNode* NzTerrainNode::GetParent()
{
    return m_parent;
}

void NzTerrainNode::HierarchicalSubdivide(unsigned int maxDepth, bool isNotReversible)
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        NazaraError("Calling uninitialized node");
        return;
    }
    #endif

    if(m_isLeaf)
    {
        if(m_nodeID.depth < maxDepth)
        {
            this->Subdivide(isNotReversible);

            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->m_doNotRefine = isNotReversible;

            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->HierarchicalSubdivide(maxDepth,isNotReversible);
        }
    }
    else
    {
        for(int i(0) ; i < 4 ; ++i)
            m_children[i]->HierarchicalSubdivide(maxDepth,isNotReversible);
    }
}

bool NzTerrainNode::IsLeaf() const
{
    return m_isLeaf;
}

bool NzTerrainNode::IsRoot() const
{
    return m_isRoot;
}

bool NzTerrainNode::IsValid() const
{
    return m_isInitialized;
}

void NzTerrainNode::Initialize(nzTerrainNodeData *data, NzTerrainNode* parent, nzNodeLocation location)
{
    InitializeData(data,parent,location);
    CreatePatch();
}

void NzTerrainNode::Invalidate()
{
    DeletePatch();
    m_isInitialized = false;
}

void NzTerrainNode::HierarchicalSlopeBasedSubdivide(unsigned int maxDepth)
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        NazaraError("Calling uninitialized node");
        return;
    }
    #endif

    //Si la cellule est une feuille
    if(m_isLeaf == true)
    {
        //Si son niveau est inférieur au niveau max de subdivision
            //Et également inférieur au niveau minimum de précision requis par la pente du terrain
                //Alors on le subdivise
        if(m_nodeID.depth < maxDepth && m_nodeID.depth < static_cast<unsigned int>(m_patch->GetGlobalSlope() * m_data->quadtree->GetMaximumHeight()))
        {
            m_doNotRefine = true;//FIX ME : Affecter cette valeur aux enfants plutot ?
            this->Subdivide();

            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->HierarchicalSlopeBasedSubdivide(maxDepth);
        }
    }
    else
    {
            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->HierarchicalSlopeBasedSubdivide(maxDepth);
    }
}

bool NzTerrainNode::Subdivide(bool isNotReversible)
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        NazaraError("Calling uninitialized node.");
        return false;
    }

    if(m_children[nzNodeLocation_topleft] != nullptr ||
       m_children[nzNodeLocation_topright] != nullptr ||
       m_children[nzNodeLocation_bottomleft] != nullptr ||
       m_children[nzNodeLocation_bottomright] != nullptr)
    {
       NazaraError("Trying to overwrite existing children.");
       return false;
    }
    #endif

    if(!m_isLeaf)
        return false;

    m_isLeaf = false;
    m_data->quadtree->UnRegisterLeaf(this);
    //this->DeletePatch();
    m_patch->UnUploadMesh();

    //On récupère des pointeurs valides pour les nodes
    m_children[nzNodeLocation_topleft] = NzDynaTerrain::GetTerrainNode();
    m_children[nzNodeLocation_topright] = NzDynaTerrain::GetTerrainNode();
    m_children[nzNodeLocation_bottomleft] = NzDynaTerrain::GetTerrainNode();
    m_children[nzNodeLocation_bottomright] = NzDynaTerrain::GetTerrainNode();

    m_children[nzNodeLocation_topleft]->Initialize(m_data,this,*m_patch,nzNodeLocation_topleft);
    m_children[nzNodeLocation_topright]->Initialize(m_data,this,*m_patch,nzNodeLocation_topright);
    m_children[nzNodeLocation_bottomleft]->Initialize(m_data,this,*m_patch,nzNodeLocation_bottomleft);
    m_children[nzNodeLocation_bottomright]->Initialize(m_data,this,*m_patch,nzNodeLocation_bottomright);

    //C'est une subdivision, le node est forcément une leaf
    m_children[nzNodeLocation_topleft]->m_isLeaf = true;
    m_children[nzNodeLocation_topright]->m_isLeaf = true;
    m_children[nzNodeLocation_bottomleft]->m_isLeaf = true;
    m_children[nzNodeLocation_bottomright]->m_isLeaf = true;

    //Et on l'enregistre auprès du quadtree
    m_data->quadtree->RegisterLeaf(m_children[nzNodeLocation_topleft]);
    m_data->quadtree->RegisterLeaf(m_children[nzNodeLocation_topright]);
    m_data->quadtree->RegisterLeaf(m_children[nzNodeLocation_bottomleft]);
    m_data->quadtree->RegisterLeaf(m_children[nzNodeLocation_bottomright]);

    //On s'assure que les nodes voisins n'aient pas plus d'1 niveau d'écart de profondeur
    m_children[nzNodeLocation_topleft]->AssertNeighborRule(nzNeighbourDirection_left,isNotReversible);
    m_children[nzNodeLocation_topleft]->AssertNeighborRule(nzNeighbourDirection_top,isNotReversible);

    m_children[nzNodeLocation_topright]->AssertNeighborRule(nzNeighbourDirection_right,isNotReversible);
    m_children[nzNodeLocation_topright]->AssertNeighborRule(nzNeighbourDirection_top,isNotReversible);

    m_children[nzNodeLocation_bottomleft]->AssertNeighborRule(nzNeighbourDirection_left,isNotReversible);
    m_children[nzNodeLocation_bottomleft]->AssertNeighborRule(nzNeighbourDirection_bottom,isNotReversible);

    m_children[nzNodeLocation_bottomright]->AssertNeighborRule(nzNeighbourDirection_right,isNotReversible);
    m_children[nzNodeLocation_bottomright]->AssertNeighborRule(nzNeighbourDirection_bottom,isNotReversible);

    //Gestion des normales entre les 4 nouveaux nodes
    m_children[nzNodeLocation_topleft]->m_patch->SetNormalsFromNeighbours(nzNeighbourDirection_right,   m_children[nzNodeLocation_topright]->m_patch);
    m_children[nzNodeLocation_topleft]->m_patch->SetNormalsFromNeighbours(nzNeighbourDirection_bottom,  m_children[nzNodeLocation_bottomleft]->m_patch);
    m_children[nzNodeLocation_bottomleft]->m_patch->SetNormalsFromNeighbours(nzNeighbourDirection_right,m_children[nzNodeLocation_bottomright]->m_patch);
    m_children[nzNodeLocation_topright]->m_patch->SetNormalsFromNeighbours(nzNeighbourDirection_bottom, m_children[nzNodeLocation_bottomright]->m_patch);

    m_children[nzNodeLocation_topleft]->m_patch->SetCornerNormalFromNeighbour(m_children[nzNodeLocation_bottomright]->m_patch);

    //Gestion des normales avec les nodes voisins
    m_children[nzNodeLocation_topright]->NeighborNormalsSmooth(nzNeighbourDirection_right);
    m_children[nzNodeLocation_bottomright]->NeighborNormalsSmooth(nzNeighbourDirection_right);
    m_children[nzNodeLocation_bottomright]->NeighborNormalsSmooth(nzNeighbourDirection_bottom);
    m_children[nzNodeLocation_bottomleft]->NeighborNormalsSmooth(nzNeighbourDirection_bottom);

    m_children[nzNodeLocation_topleft]->NeighborNormalsChanged(nzNeighbourDirection_left);
    m_children[nzNodeLocation_bottomleft]->NeighborNormalsChanged(nzNeighbourDirection_left);
    m_children[nzNodeLocation_topleft]->NeighborNormalsChanged(nzNeighbourDirection_top);
    m_children[nzNodeLocation_topright]->NeighborNormalsChanged(nzNeighbourDirection_top);

    m_children[nzNodeLocation_topleft]->m_patch->UploadMesh(false);
    m_children[nzNodeLocation_topright]->m_patch->UploadMesh(false);
    m_children[nzNodeLocation_bottomleft]->m_patch->UploadMesh(false);
    m_children[nzNodeLocation_bottomright]->m_patch->UploadMesh(false);

    return true;
}

//METHODE A NETTOYER ET SIMPLIFIER (SUPPRIMER LES BOUCLES, LES TABLEAUX ETC)
bool NzTerrainNode::Refine()
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        NazaraError("Calling uninitialized node");
        return false;
    }
    #endif

    //Impossible de refiner une feuille
    if(m_isLeaf)
        return false;

    //Impossible de refiner autre chose qu'un parent d'une feuille
    if(!m_children[0]->m_isLeaf || !m_children[1]->m_isLeaf || !m_children[2]->m_isLeaf || !m_children[3]->m_isLeaf)
        return false;

    if(m_children[0]->m_doNotRefine || m_children[1]->m_doNotRefine ||
       m_children[2]->m_doNotRefine || m_children[3]->m_doNotRefine)
            return false;

    nzNeighbourDirection first, second;
    NzTerrainNode* temp = nullptr;

    nzNeighbourDirection dirDirection[4] = {nzNeighbourDirection_top,
                                            nzNeighbourDirection_bottom,
                                            nzNeighbourDirection_left,
                                            nzNeighbourDirection_right};

    nzNeighbourDirection invDirection[4] = {nzNeighbourDirection_bottom,
                                            nzNeighbourDirection_top,
                                            nzNeighbourDirection_right,
                                            nzNeighbourDirection_left};

    //Contains location of neighbor's children in contact with (*this)
    nzNodeLocation locLUT[4][2] =  {{nzNodeLocation_bottomleft,nzNodeLocation_bottomright},//TOP Neighbour
                                    {nzNodeLocation_topleft   ,nzNodeLocation_topright},//BOTTOM Neighbour
                                    {nzNodeLocation_topright  ,nzNodeLocation_bottomright},//LEFT Neighbour
                                    {nzNodeLocation_topleft   ,nzNodeLocation_bottomleft}};//RIGHT Neighbour

    //Impossible de refiner si les voisins ne sont pas d'accord
    for(int i(0) ; i < 4 ; ++i)
    {
        //On récupère les directions à tester en fonction de la position du node fils
        Split(m_children[i]->m_location,first,second);

        temp = m_children[i]->GetDirectNeighbor(first);
        //TODO : tester la validité du node
        //Si il y a un node voisin de niveau égal
        if(temp != nullptr)
        {
             //Si il des fils
             if(!temp->m_isLeaf)
                return false;//Abandon, le refine va causer une différence de profondeur > 2
        }

        temp = m_children[i]->GetDirectNeighbor(second);
        //Si il y a un node voisin de niveau égal
        if(temp != nullptr)
        {
             //Si il des fils
             if(!temp->m_isLeaf)
                return false;//Abandon, le refine va causer une différence de profondeur > 2
        }
    }

    //On supprime les fils
    for(int i(0) ; i < 4 ; ++i)
    {
        m_data->quadtree->UnRegisterLeaf(m_children[i]);
        m_children[i]->Invalidate();
        m_data->quadtree->DeleteNode(m_children[i]);
        m_children[i] = nullptr;
    }

    //Ce node devient leaf
    m_isLeaf = true;
    m_data->quadtree->RegisterLeaf(this);
    nzNeighbourDirection direct;
    m_patch->Reset();

    for(int i(0) ; i < 4 ; ++i)
    {
        //On signale aux voisins le refinement
        temp = this->GetDirectNeighbor(dirDirection[i]);
        direct = static_cast<nzNeighbourDirection>(i);//TOP, BOTTOM, LEFT, RIGHT

        if(temp != nullptr)
        {
            //On en profite pour vérifier si le node voisin fait partie d'un autre quadtree
            //auquel cas on récupère les directions directe et inverse (pas nécessairement opposées)
            if(temp->m_data->quadtree != m_data->quadtree)
            {
                invDirection[i] = temp->m_data->quadtree->GetNeighbourDirection(m_data->quadtree);
                direct = invDirection[i];
                direct = Reverse(direct);
            }

            if(temp->m_isLeaf)
            {
                //This et son voisin auront le même niveau, on supprime l'interface ?
                temp->m_patch->SetConfiguration(invDirection[i],0);
            }
            else
            {
                //This aura un niveau inférieur, on indique aux fils du voisin de générer une interface
                if(temp->m_children[locLUT[direct][0]]->m_isLeaf)
                {
                    temp->m_children[locLUT[direct][0]]->m_patch->SetConfiguration(invDirection[i],1);
                }

                if(temp->m_children[locLUT[direct][1]]->m_isLeaf)
                {
                    temp->m_children[locLUT[direct][1]]->m_patch->SetConfiguration(invDirection[i],1);
                }
            }

        }
    }
    //CreatePatch();
    m_patch->UploadMesh();

    return true;

}

bool NzTerrainNode::HierarchicalRefine()
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        NazaraError("Calling uninitialized node");
        return false;
    }
    #endif

    //Impossible de refiner une feuille
    if(m_isLeaf)
        return false;

    //On refine les enfants d'abord
    for(int i(0) ; i < 4 ; ++i)
    {
        if(!m_children[i]->m_isLeaf)
        {
            m_children[i]->HierarchicalRefine();
        }
    }

    return this->Refine();

}

void NzTerrainNode::AssertNeighborRule(nzNeighbourDirection direction, bool isNotReversible)
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        NazaraError("Calling uninitialized node");
        return;
    }
    #endif

    NzTerrainNodeID tempID = m_nodeID;
    nzNeighbourDirection invDirection = direction;

    switch(direction)
    {
        case nzNeighbourDirection_top :
            tempID.locy -= 1;
            invDirection = nzNeighbourDirection_bottom;
        break;

        case nzNeighbourDirection_right :
            tempID.locx += 1;
            invDirection = nzNeighbourDirection_left;
        break;

        case nzNeighbourDirection_bottom :
            tempID.locy += 1;
            invDirection = nzNeighbourDirection_top;
        break;

        case nzNeighbourDirection_left :
            tempID.locx -= 1;
            invDirection = nzNeighbourDirection_right;
        break;

        default:
        break;
    }

    NzTerrainQuadTree* tempQuad;
    NzTerrainNode* tempNode;

    //Si on ne cherche pas à atteindre une case externe
    if(tempID.IsValid())
    {
        tempQuad = m_data->quadtree;
        tempNode = m_data->quadtree->GetNode(tempID);
    }
    else
    {
        tempQuad = m_data->quadtree->GetNeighbourQuadTree(direction);

        if(tempQuad == nullptr)
            return;

        nzConnectionType type = m_data->quadtree->GetConnectionType(tempQuad);
        switch(type)
        {
            case nzConnectionType_orthogonal :
                tempID = m_nodeID;
                tempID.InvertXY();
                break;

            case nzConnectionType_straight :
                tempID.Normalize();
                break;

            case nzConnectionType_reverse :
                tempID = m_nodeID;
                break;
        }

        tempNode = tempQuad->GetNode(tempID);
        invDirection = tempQuad->GetNeighbourDirection(m_data->quadtree);
    }


    ///Si le voisin existe (il y a un node voisin de même profondeur)
    if(tempNode != nullptr)
    {
        //La subdivision a supprimé une interface de précision, on l'indique au voisin qu'il n'a plus besoin de s'adapter
        //TODO : Ce test ne devrait jamais rentrer dans le else, pourtant si, à investiguer
        if(tempNode->m_isLeaf)
        {
            tempNode->m_patch->SetConfiguration(invDirection,0);
        }
        return;
    }

    //On monte d'un niveau dans l'arbre
    --tempID;
    tempNode = tempQuad->GetNode(tempID);

    ///Si il y a un node voisin à 1 niveau plus haut dans l'arbre, la subdivision a engendré une interface
    if(tempNode != nullptr)
    {

        m_patch->SetConfiguration(direction,1);
        return;
    }

    //On monte d'un niveau dans l'arbre
    --tempID;
    tempNode = tempQuad->GetNode(tempID);

    ///Si il y a un node voisin 2 niveaux plus haut dans l'arbre
        //Il faut subdiviser ce node une fois pour réduire l'écart à 1
        //et respecter la règle du "1 niveau d'écart max avec les voisins"
        //Il faut indiquer une interface de précision car écart non nul
    //L'entrée dans cette portion de code est normale, certes l'écart initial est de 2 > 1,
    //mais il est immédiatement corrigé
    if(tempNode != nullptr)
    {
        //On subdivise la cellule jusqu'à atteindre le bon niveau
        tempNode->HierarchicalSubdivide(m_nodeID.depth-1,isNotReversible);
        //La subdivision a généré une interface, le node le plus subdivisé (cad this) doit s'adapter
        m_patch->SetConfiguration(direction,1);
        return;
    }

    //Si on est ici c'est qu'il y a node voisin à au moins 3 niveaux au dessus
    //Ce qui brise la règle du "1 niveau d'écart max avec les voisins"
    #if NAZARA_DYNATERRAIN_SAFE
    --tempID;
    tempNode = tempQuad->GetNode(tempID);
    int counter = 0;
    while(tempNode == nullptr && counter < 10)
    {
        counter++;
        tempID.depth -= 1;
        tempID.locx /= 2;
        tempID.locy /= 2;
        tempNode = tempQuad->GetNode(tempID);
    }

    tempNode->HierarchicalSubdivide(m_nodeID.depth-1,isNotReversible);
    m_patch->SetConfiguration(direction,1);
    NazaraError("Neighbour node at " + NzString::Number(counter + 3) +  " levels higher than actual node, terrain state might be corrupted");
    #endif
}

void NzTerrainNode::NeighborNormalsSmooth(nzNeighbourDirection direction)
{
    NzTerrainNode* neighbour = GetDirectNeighbor(direction);

    if(neighbour == nullptr)
    {
        neighbour = m_parent->GetDirectNeighbor(direction);

        if(neighbour == nullptr)
            return;

    }

    if(neighbour->IsLeaf())
    {
        m_patch->SetNormalsFromNeighbours(direction, neighbour->m_patch);
    }
    else
    {
        NzTerrainNode *node1, *node2;
        switch(direction)
        {
            case nzNeighbourDirection_top :
                node1 = neighbour->m_children[nzNodeLocation_bottomleft];
                node2 = neighbour->m_children[nzNodeLocation_bottomright];
            break;

            case nzNeighbourDirection_right :
                node1 = neighbour->m_children[nzNodeLocation_topleft];
                node2 = neighbour->m_children[nzNodeLocation_bottomleft];
            break;

            case nzNeighbourDirection_bottom :
                node1 = neighbour->m_children[nzNodeLocation_topright];
                node2 = neighbour->m_children[nzNodeLocation_topleft];
            break;

            case nzNeighbourDirection_left :
                node1 = neighbour->m_children[nzNodeLocation_topright];
                node2 = neighbour->m_children[nzNodeLocation_bottomright];
            break;
        }

        m_patch->SetNormalsFromNeighbours(direction,node1->m_patch,node2->m_patch);
    }
}

void NzTerrainNode::NeighborNormalsChanged(nzNeighbourDirection direction)
{
    NzTerrainNode* neighbour = GetDirectNeighbor(direction);

    if(neighbour == nullptr)
    {
        neighbour = m_parent->GetDirectNeighbor(direction);

        if(neighbour == nullptr)
            return;

    }
    if(neighbour->IsLeaf())
    {
        neighbour->m_patch->SetNormalsFromNeighbours(Reverse(direction),m_patch);
        neighbour->m_patch->UploadMesh(false);
    }
    else
    {
        NzTerrainNode *node1, *node2;
        switch(direction)
        {
            case nzNeighbourDirection_top :
                node1 = neighbour->m_children[nzNodeLocation_bottomleft];
                node2 = neighbour->m_children[nzNodeLocation_bottomright];
            break;

            case nzNeighbourDirection_right :
                node1 = neighbour->m_children[nzNodeLocation_topleft];
                node2 = neighbour->m_children[nzNodeLocation_bottomleft];
            break;

            case nzNeighbourDirection_bottom :
                node1 = neighbour->m_children[nzNodeLocation_topright];
                node2 = neighbour->m_children[nzNodeLocation_topleft];
            break;

            case nzNeighbourDirection_left :
                node1 = neighbour->m_children[nzNodeLocation_topright];
                node2 = neighbour->m_children[nzNodeLocation_bottomright];
            break;
        }

        node1->m_patch->SetNormalsFromNeighbours(Reverse(direction),m_patch);
        node2->m_patch->SetNormalsFromNeighbours(Reverse(direction),m_patch);
    }

}

void NzTerrainNode::Update(const NzVector3f& cameraPosition)
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        NazaraError("Calling uninitialized node");
        return;
    }
    #endif

    //A) On calcule la précision optimale du node tenant compte de sa distance à la caméra
    float distance = m_aabb.aabb.DistanceTo(cameraPosition);

    int rayon = NzDynaTerrain::GetPrecisionLevelFromDistance(distance);

    //B) Si la précision optimale est inférieure à la précision actuelle
        //Si le node est une feuille, on l'ajoute à la liste de subdivision
        //Sinon on update ses enfants
            //Le fusion pouvant échouer, on garantit que le node subdivisé ne reste pas dans la file de fusion
    if(m_nodeID.depth < rayon)
    {
        if(m_isLeaf)
        {
            m_data->quadtree->AddLeaveToSubdivisionQueue(this);
        }
        else
        {
            m_data->quadtree->TryRemoveNodeFromRefinementQueue(this);

            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->Update(cameraPosition);
        }
    }
    //C) Si la précision optimale est supérieure ou égale à la précision actuelle
        //Si le node n'est pas une feuille, on l'ajoute à la liste de fusion
    else if(m_nodeID.depth >= rayon)
    {
        if(!m_isLeaf)
        {
            m_data->quadtree->AddNodeToRefinementQueue(this);
        }
    }
}

void NzTerrainNode::Initialize(nzTerrainNodeData *data, NzTerrainNode* parent, const NzPatch& parentPatch, nzNodeLocation location)
{
    InitializeData(data,parent,location);
    m_patch = NzDynaTerrain::GetTerrainPatch();
    m_patch->InitializeFromParent(m_nodeID,m_data,parentPatch);
    m_aabb = m_patch->GetAABB();
    m_patch->UploadMesh();
}

void NzTerrainNode::InitializeData(nzTerrainNodeData *data, NzTerrainNode* parent, nzNodeLocation location)
{
    m_isInitialized = true;
    m_data = data;
    m_location = location;
    m_isLeaf = false;

    m_doNotRefine = false;

    for(int i(0) ; i < 4 ; ++i)
        m_children[i] = nullptr;

    if(parent == nullptr)
    {
        m_isRoot = true;
        m_isLeaf = true;
        m_nodeID.depth = 0;
        m_nodeID.locx = 0;
        m_nodeID.locy = 0;
    }
    else
    {
        m_nodeID.depth = parent->m_nodeID.depth + 1;
        m_parent = parent;
        m_isRoot = false;
        int offx = 0, offy = 0;
        switch(m_location)
        {
            case nzNodeLocation_topright:
                offx = 1;
            break;

            case nzNodeLocation_bottomleft:
                offy = 1;
            break;

            case nzNodeLocation_bottomright:
                offx = 1;
                offy = 1;
            break;

            case nzNodeLocation_topleft:
            default:
            break;
        }
        m_nodeID.locx = parent->m_nodeID.locx * 2 + offx;
        m_nodeID.locy = parent->m_nodeID.locy * 2 + offy;
    }
}
