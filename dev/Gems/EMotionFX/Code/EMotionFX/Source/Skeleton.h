/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

// include the Core system
#include "EMotionFXConfig.h"
#include "BaseObject.h"
#include "Pose.h"


namespace EMotionFX
{
    // forward declarations
    class Node;
    class Transform;


    class EMFX_API Skeleton
        : public BaseObject
    {
        MCORE_MEMORYOBJECTCATEGORY(Skeleton, EMFX_DEFAULT_ALIGNMENT, EMFX_MEMCATEGORY_SKELETON);
    public:

        static Skeleton* Create();

        Skeleton* Clone();

        MCORE_INLINE uint32 GetNumNodes() const                 { return mNodes.GetLength(); }
        MCORE_INLINE Node* GetNode(uint32 index) const          { return mNodes[index]; }

        void ReserveNodes(uint32 numNodes);
        void AddNode(Node* node);
        void RemoveNode(uint32 nodeIndex, bool delFromMem = true);
        void RemoveAllNodes(bool delFromMem = true);

        MCORE_INLINE const Pose* GetBindPose() const            { return &mBindPose; }
        MCORE_INLINE Pose* GetBindPose()                        { return &mBindPose; }

        /**
         * Search for a node by name (case sensitive).
         * @param name The name of the node (case sensitive).
         * @return A pointer to the node, returns nullptr when not found.
         */
        Node* FindNodeByName(const char* name) const;

        /**
         * Search for a node by name (non case sensitive), returns nullptr when no node can be found.
         * Note: when there are two nodes with the same non case sensitive name, the first one is returned.
         * So if there is a node named "Blah" and one named "BLAH" and we search on "blah", the first one ("Blah") is returned.
         * @param name The name of the node, non case sensitive.
         * @result A pointer to the node with the given name, or nullptr when none could be found.
         */
        Node* FindNodeByNameNoCase(const char* name) const;

        /**
         * Search for a node on ID. Returns nullptr when no node found.
         * A node ID is based on the name. So all nodes (also inside other actors) with the same name will have the same ID.
         * This can be used for faster searching, compared to searching by name, which uses string compares.
         * @param id The ID to search for.
         * @return A pointer to the node, or nullptr when no node with the given ID found.
         */
        Node* FindNodeByID(uint32 id) const;

        /**
         * Set the value of a given node.
         * @param index The node number, which must be in range of [0..GetNumNodes()-1].
         * @param node The node value to set at this index.
         */
        void SetNode(uint32 index, Node* node);

        /**
         * Set the number of nodes.
         * This resizes the array of pointers to nodes, but doesn't actually create the nodes.
         * @param numNodes The number of nodes to allocate space for.
         */
        void SetNumNodes(uint32 numNodes);

        /**
         * Update all the node index values that are returned by the Node::GetNodeIndex() method.
         * When the order or layout of the nodes array changes, for example after removing a node, the index values of
         * the nodes have to be updated. As node number 5 could become node number 4 in the example case.
         * @param startNode The node number to start updating from.
         */
        void UpdateNodeIndexValues(uint32 startNode = 0);

        /**
         * Get the number of root nodes in the actor. A root node is a node without any parent.
         * @result The number of root nodes inside the actor.
         */
        MCORE_INLINE uint32 GetNumRootNodes() const                             { return mRootNodes.GetLength(); }

        /**
         * Get the node number/index of a given root node.
         * @param nr The root node number, which must be in range of [0..GetNumRootNodes()-1].
         * @result The node index of the given root node.
         */
        MCORE_INLINE uint32 GetRootNodeIndex(uint32 nr) const                   { return mRootNodes[nr]; }

        /**
         * Pre-allocate space for the root nodes array.
         * This does not alter the value returned by GetNumRootNodes() though.
         * @param numNodes The absolute number of nodes to pre-allocate space for.
         */
        void ReserveRootNodes(uint32 numNodes);

        /**
         * Add a root node to the actor.
         * This doesn't modify the node itself, but it will add the node to the list of root nodes.
         * @param nodeIndex The node number/index to add and mark as root node inside the actor.
         */
        void AddRootNode(uint32 nodeIndex);

        /**
         * Remove a given root node from the list of root nodes stored inside the actor.
         * This doesn't really remove the node itself, but it just unregisters it as root node inside the actor.
         * @param nr The root node to remove, which must be in range of [0..GetNumRootNodes()-1].
         */
        void RemoveRootNode(uint32 nr);

        /**
         * Removes all root nodes from the actor.
         * This doesn't remove the actual nodes, but it just unregisters them as root nodes, so that the GetNumRootNodes()
         * method will return a value of zero.
         */
        void RemoveAllRootNodes();

        /**
         * Calculate the local space matrix for a given node, from a set of local space transforms (separated position/rotation/scale/scalerotation).
         * @param nodeIndex The node to calculate the local space matrix for. This must be in range of [0..Actor::GetNumNodes()-1].
         * @param localTransforms The local space transformations of all nodes. You can get those with the Actor::GetTransformData().GetOrgTransforms() for example.
         * @param outMatrix The matrix to store the resulting local space matrix in.
         */
        void CalcLocalSpaceMatrix(uint32 nodeIndex, const Transform* localTransforms, MCore::Matrix* outMatrix);

        /**
         * Calculate the local space matrices for all nodes, in bind pose. Bind pose is the pose in which the actor has been exported from the art package.
         * @param outLocalMatrices The array of matrices that will contain the local space matrices for all nodes. This array will automatically be resized to contain Actor::GetNumNodes() matrices.
         */
        void CalcBindPoseLocalMatrices(MCore::Array<MCore::Matrix>& outLocalMatrices);

        /**
         * Calculate the global space matrices from a set of local space matrices.
         * @param localMatrices The local space matrices. This array must contain Actor::GetNumNodes() matrices.
         * @param outGlobalMatrices The array of global space matrices to write the results in. The array will automatically be resized to the right number of elements by this method.
         */
        void CalcGlobalMatrices(const MCore::Array<MCore::Matrix>& localMatrices, MCore::Array<MCore::Matrix>& outGlobalMatrices);

        /**
         * Calculate the global space matrices of the bind pose of this actor.
         * The bind pose is the pose in which the actor has been exported from the art package.
         * The number of matrices inside this array will be equal to Actor::GetNumNodes() after execution of this method.
         * @param outGlobalMatrices The array of matrices to write the resulting global space matries in. The array will automatically be resized to the number of nodes inside this actor.
         */
        void CalcBindPoseGlobalMatrices(MCore::Array<MCore::Matrix>& outGlobalMatrices);

        void LogNodes();
        uint32 CalcHierarchyDepthForNode(uint32 nodeIndex) const;

    private:
        MCore::Array<Node*>     mNodes;         /**< The nodes, including root nodes. */
        MCore::Array<uint32>    mRootNodes;     /**< The root nodes only. */
        Pose                    mBindPose;      /**< The bind pose. */

        Skeleton();
        ~Skeleton();
    };
}   // namespace EMotionFX
