//
// Created by sleepwalker on 2024/1/20.
//

#ifndef DEMO_AVLTREE_H
#define DEMO_AVLTREE_H
#include <memory>
#include <vector>
namespace trees
{
    template<typename T>
    class AvlTree
    {
    private:
        struct Node
        {
            std::shared_ptr<T> data;
            Node* left_child;
            Node* right_child;
            int height;
            explicit Node(T d)
            {
                data = std::make_shared<T>(std::move(d));
                left_child = nullptr;
                right_child = nullptr;
                height = 0;
            }
        };
        Node* root;
        void Insert(T data, Node* &node)
        {
            if(node == nullptr)
            {
                node = new Node(std::move(data));
                return;
            }
            if(data < *node->data)
            {
                Insert(std::move(data), node->left_child);
            }
            else
            {
                Insert(std::move(data), node->right_child);
            }
            balance(node);
        }

        Node**  Find(T& data, Node* &node)
        {
            if (node == nullptr)
            {
                return nullptr;
            }

            if (data == *node->data)
            {
                return &node;
            }
            else if (data < *node->data)
            {
                return Find(data, node->left_child);
            }
            else
            {
                return Find(data, node->right_child);
            }
        }

        bool isLeaf(Node* node)
        {
            return !node->left_child && !node->right_child;
        }

        Node** FindMin(Node* &node)
        {
            if(node->left_child)
            {
                return FindMin(node->left_child);
            }
            else
            {
                return &node;
            }
        }


        void ForeVisit(Node* node, std::vector<T> &res)
        {
            if(node->left_child)
            {
                ForeVisit(node->left_child, res);
            }
            res.push_back(*node->data);
            if(node->right_child)
            {
                ForeVisit(node->right_child, res);
            }
        }

        void Delete(T& data, Node* &node)
        {
            if(node == nullptr)
            {
                return;
            }

            if(data < *node->data)
            {
                Delete(data, node->left_child);
            }
            else if(data > *node->data)
            {
                Delete(data, node->right_child);
            }
            else if(node->left_child && node->right_child)
            {
                auto min = FindMin(node->right_child);
                node->data = (*min)->data;
                Delete(*(*min)->data, node->right_child);
            }
            else
            {
                auto temp = node;
                node = node->left_child? node->left_child : node->right_child;
                delete temp;
            }
            balance(node);
        }

        void RotateWithLeftChild(Node* &node)
        {
            auto k1 = node->left_child;
            node->left_child = k1->right_child;
            k1->right_child = node;
            node->height = std::max(height(node->right_child), height(node->left_child)) + 1;
            k1->height = std::max(height(k1->left_child), height(k1->right_child)) + 1;
            node = k1;
        }

        void RotateWithRightChild(Node* &node)
        {
            auto k1 = node->right_child;
            node->right_child = k1->left_child;
            k1->left_child = node;
            node->height = std::max(height(node->left_child), height(node->right_child)) + 1;
            k1->height = std::max(height(k1->left_child), height(k1->right_child)) + 1;
            node = k1;
        }

        void DoubleRotateWithLeftChild(Node* &node)
        {
            RotateWithRightChild(node->left_child);
            RotateWithLeftChild(node);
        }

        void DoubleRotateWithRightChild(Node* &node)
        {
            RotateWithLeftChild(node->right_child);
            RotateWithRightChild(node);
        }

        int height(Node* node)
        {
            return node == nullptr? -1: node->height;
        }

        void balance(Node* &node)
        {
            if(node == nullptr)
            {
                return;
            }
            if(height(node->left_child) - height(node->right_child) > 1)
            {
                if(height(node->left_child->left_child) >= height(node->left_child->right_child))
                {
                    RotateWithLeftChild(node);
                }
                else
                {
                    DoubleRotateWithLeftChild(node);
                }
            }
            else if (height(node->right_child) - height(node->left_child) > 1)
            {
                if(height(node->right_child->left_child) <= height(node->right_child->right_child))
                {
                    RotateWithRightChild(node);
                }
                else
                {
                    DoubleRotateWithRightChild(node);
                }
            }
            node->height = std::max(height(node->left_child), height(node->right_child)) + 1; //if no rotate, still need to refresh height
        }

    public:
        AvlTree(): root(nullptr){};
        void Insert(T data)
        {
            Insert(std::move(data), root);
        }

        std::shared_ptr<T> Find(T data)
        {
            if(auto node = Find(data, root))
            {
                return (*node)->data;
            }
            return nullptr;
        }

        void Delete(T data)
        {
            Delete(data, root);
            /*
            if(auto node = Find(data, root))
            {
                if (isLeaf(*node))
                {
                    delete *node;
                    *node = nullptr;
                }
                else if ((*node)->left_child && (*node)->right_child)
                {
                    auto minNode = FindMin((*node)->right_child);
                    (*node)->data = (*minNode)->data;
                    auto oldNode = *minNode;
                    if((*minNode)->right_child)
                    {
                        *minNode = (*minNode)->right_child;
                    }
                    else
                    {
                        *minNode = nullptr;
                    }
                    delete oldNode;
                }
                else
                {
                    auto oldNode = *node;
                    *node = oldNode->left_child ? oldNode->left_child : oldNode->right_child;
                    delete oldNode;
                }
            }
             */
        }

        std::vector<T> GetSortedData()
        {
            std::vector<T> res;
            ForeVisit(root, res);
            return res;
        }
    };
}
#endif //DEMO_AVLTREE_H
