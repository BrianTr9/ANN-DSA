/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AbstractGraph.h
 * Author: LTSACH
 *
 * Created on 23 August 2020, 17:48
 */

#ifndef ABSTRACTGRAPH_H
#define ABSTRACTGRAPH_H
#include "graph/IGraph.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;


template<class T>
class AbstractGraph: public IGraph<T>{
public:
    class Edge; //forward declaration
    class VertexNode; //forward declaration
    class Iterator; //forward declaration
    
private:
protected:
    //Using the adjacent list technique, so need to store list of nodes (nodeList)
    DLinkedList<VertexNode*> nodeList; 
    
    //Function pointers:
    bool (*vertexEQ)(T&, T&); //to compare two vertices
    string (*vertex2str)(T&); //to obtain string representation of vertices
    
    
    VertexNode* getVertexNode(T& vertex){
        typename DLinkedList<VertexNode*>::Iterator it = nodeList.begin();
        while(it != nodeList.end()){
            VertexNode* node = *it;
            // vertexEQ is optional (default = 0): fall back on operator==
            // (calling a null function pointer used to crash here)
            if(vertexEQ != 0 ? vertexEQ(node->vertex, vertex) : (node->vertex == vertex)) return node;
            it++;
        }
        return 0;
    }
    /*
     * vertexToString: string of a vertex, used in exception messages.
     * vertex2str is optional (default = 0): fall back on operator<<
     */
    string vertexToString(T& vertex){
        if(this->vertex2str != 0) return this->vertex2str(vertex);
        stringstream os;
        os << vertex;
        return os.str();
    }
    string vertex2Str(VertexNode& node){
        return vertexToString(node.vertex);
    }
    string edge2Str(Edge& edge){
        stringstream os;
        os << "E("
                << vertexToString(edge.from->vertex)
                << ","
                << vertexToString(edge.to->vertex)
                << ")";
        return os.str();
    }
    
public:
    AbstractGraph(
            bool (*vertexEQ)(T&, T&)=0, 
            string (*vertex2str)(T&)=0){
        
        this->vertexEQ = vertexEQ;
        this->vertex2str = vertex2str;
        // The graph owns its VertexNode objects: free them (and, through VertexNode's
        // adjacency list, their edges) when the graph is cleared/destroyed.
        // They used to leak.
        this->nodeList.setDeleteUserDataPtr(&DLinkedList<VertexNode*>::free);
    }
    virtual ~AbstractGraph(){}

    // The graph owns raw VertexNode pointers => a (shallow) copy would free them twice.
    AbstractGraph(const AbstractGraph<T>&) = delete;
    AbstractGraph<T>& operator=(const AbstractGraph<T>&) = delete;
    
    typedef bool (*vertexEQFunc)(T&, T&);
    typedef string (*vertex2strFunc)(T&);
    vertexEQFunc getVertexEQ(){
        return this->vertexEQ;
    }
    vertex2strFunc getVertex2Str(){
        return this->vertex2str;
    }

    
//////////////////////////////////////////////////////////////////////
//////////////////// IMPLEMENTATION of IGraph API ////////////////////
//////////////////////////////////////////////////////////////////////
    /* connect, disconnect, and remove: will be overridden in:
     *  >> UGraphModel
     *  >> DGraphModel
     */
    virtual void connect(T from, T to, float weight=0) =0;
    virtual void disconnect(T from, T to)=0;
    virtual void remove(T vertex)=0;
    
    
    
    /* The following are common methods for UGraphModel and DGraphModel
     */
    virtual void add(T vertex) {
        //TODO
        if (!contains(vertex)) {
            VertexNode* newNode = new VertexNode(vertex, this->vertexEQ, this->vertex2str);
            nodeList.add(newNode);
        }
    }
    virtual bool contains(T vertex){
        //TODO
        typename DLinkedList<VertexNode*>::Iterator it = nodeList.begin();
        while (it != nodeList.end()) {
            if (vertexEQ == nullptr) {
                if ((*it)->vertex == vertex) return true;
            } else {
                if (vertexEQ((*it)->vertex, vertex)) return true;
            }
            it++;
        }
        return false;
    }
    virtual float weight(T from, T to){
        //TODO
        VertexNode* fromNode = getVertexNode(from);
        VertexNode* toNode = getVertexNode(to);

        if (fromNode == nullptr) {
            throw VertexNotFoundException(vertexToString(from));
        }
        if (toNode == nullptr) {
            throw VertexNotFoundException(vertexToString(to));
        }

        Edge* edge = fromNode->getEdge(toNode);
        if (edge == nullptr) {
            throw EdgeNotFoundException("E(" + this->vertexToString(from) + "," + this->vertexToString(to) + ")");
        }

        return edge->weight;
    }
    virtual DLinkedList<T> getOutwardEdges(T from){
        //TODO
        VertexNode* fromNode = getVertexNode(from);
        if (fromNode == nullptr) {
            throw VertexNotFoundException(vertexToString(from));
        }
        return fromNode->getOutwardEdges();
    }
    
    virtual DLinkedList<T> getInwardEdges(T to){
        //TODO
        VertexNode* toNode = getVertexNode(to);
        if (toNode == nullptr) {
            throw VertexNotFoundException(vertexToString(to));
        }
        DLinkedList<T> inwardEdges;
        typename DLinkedList<VertexNode*>::Iterator it = nodeList.begin();
        while(it != nodeList.end()){
            VertexNode* node = *it;
            Edge* edge = node->getEdge(toNode);
            if(edge != nullptr) inwardEdges.add(node->vertex);
            it++;
        }
        return inwardEdges;
    }
    
    virtual int size() {
        //TODO
        return nodeList.size();
    }
    virtual bool empty(){
        //TODO
        return nodeList.size() == 0;
    }
    virtual void clear(){
        //TODO
        // nodeList.clear() deletes every VertexNode (free), whose destructor deletes its edges
        nodeList.clear();
    }
    virtual int inDegree(T vertex){
        //TODO
        VertexNode* node = getVertexNode(vertex);
        if (node == nullptr) {
            throw VertexNotFoundException(vertexToString(vertex));
        }
        return node->inDegree();
    }
    virtual int outDegree(T vertex){
        //TODO
        VertexNode* node = getVertexNode(vertex);
        if (node == nullptr) {
            throw VertexNotFoundException(vertexToString(vertex));
        }
        return node->outDegree();
    }
    
    virtual DLinkedList<T> vertices(){
        //TODO
        DLinkedList<T> vertexList;
        typename DLinkedList<VertexNode*>::Iterator it = nodeList.begin();
        while(it != nodeList.end()){
            vertexList.add((*it)->vertex);
            it++;
        }
        return vertexList;
    }
    virtual bool connected(T from, T to){
        //TODO
        VertexNode* fromNode = getVertexNode(from);
        VertexNode* toNode = getVertexNode(to);

        if (fromNode == nullptr) {
            throw VertexNotFoundException(vertexToString(from));
        }
        if (toNode == nullptr) {
            throw VertexNotFoundException(vertexToString(to));
        }

        return fromNode->getEdge(toNode) != nullptr;
    }
    void println(){
        cout << this->toString() << endl;
    }
    virtual string toString(){
        string mark(50, '=');
        stringstream os;
        os << mark << endl;
        os << left << setw(12) << "Vertices:" << endl;
        typename DLinkedList<VertexNode*>::Iterator nodeIt = nodeList.begin();
        while(nodeIt != nodeList.end()){
            VertexNode* node = *nodeIt;
            os << node->toString() << endl;
            nodeIt++;
        }
        string mark2(30, '-');
        os << mark2 << endl;
        
        os << left << setw(12) << "Edges:" << endl;
        
        nodeIt = nodeList.begin();
        while(nodeIt != nodeList.end()){
            VertexNode* node = *nodeIt;
            
            typename DLinkedList<Edge*>::Iterator edgeIt = node->adList.begin();
            while(edgeIt != node->adList.end()){
                Edge* edge = *edgeIt;
                os << edge->toString() << endl;
                
                edgeIt++;
            }
            
            nodeIt++;
        }
        os << mark << endl;
        return os.str();
    }
    
    /* begin, end: will be used to traverse on graph's vertices
     * example: assume that "graph" is a graph.
     * Code:
     *      AbstractGraph<T>::Iterator it;
     *      for(it = graph.begin(); it != graph.end(); it++){
     *          T vertex = *it; 
     *          //continue to process vertex here!
     *      }
     */
    Iterator begin(){
        return Iterator(this, true);
    }
    Iterator end(){
        return Iterator(this, false);
    }

//////////////////////////////////////////////////////////////////////
////////////////////////  INNER CLASSES DEFNITION ////////////////////
//////////////////////////////////////////////////////////////////////
    
public:
//BEGIN of VertexNode    
    class VertexNode{
    private:
        template<class U>
        friend class UGraphModel; //UPDATED: added
        T vertex;
        int inDegree_, outDegree_;
        DLinkedList<Edge*> adList; 
        friend class Edge;
        friend class AbstractGraph;
        
        bool (*vertexEQ)(T&, T&);
        string (*vertex2str)(T&);
        
    public:
        VertexNode():adList(&DLinkedList<Edge*>::free, &Edge::edgeEQ){
            this->inDegree_ = this->outDegree_ = 0;
            this->vertexEQ = 0;
            this->vertex2str = 0;
        }
        VertexNode(T vertex, bool (*vertexEQ)(T&, T&), string (*vertex2str)(T&))
            :adList(&DLinkedList<Edge*>::free, &Edge::edgeEQ){
            this->vertex = vertex;
            this->vertexEQ = vertexEQ;
            this->vertex2str = vertex2str;
            this->outDegree_ = this->inDegree_ = 0;
        }
        T& getVertex(){
            return vertex;
        }
        void connect(VertexNode* to, float weight=0){
            //TODO
            typename DLinkedList<Edge*>::Iterator it = adList.begin();
            while(it != adList.end()){
                if (vertexEQ == nullptr) {
                    if((*it)->to->vertex == to->vertex) {
                        (*it)->weight = weight;
                        return;
                    }
                } else {
                    if(vertexEQ((*it)->to->vertex, to->vertex)) { 
                        (*it)->weight = weight;
                        return;
                    };
                }
                it++;
            }

            Edge* newEdge = new Edge(this, to, weight);
            this->adList.add(newEdge);
            this->outDegree_++;
            to->inDegree_++;
        }
        DLinkedList<T> getOutwardEdges(){
            //TODO
            DLinkedList<T> outwardEdges;
            typename DLinkedList<Edge*>::Iterator it = adList.begin();
            while(it != adList.end()){
                outwardEdges.add((*it)->to->vertex);
                it++;
            }
            return outwardEdges;
        }

        Edge* getEdge(VertexNode* to){
            //TODO
            typename DLinkedList<Edge*>::Iterator it = adList.begin();
            while(it != adList.end()){
                if (vertexEQ == nullptr) {
                    if((*it)->to->vertex == to->vertex) return *it;
                } else {
                    if(vertexEQ((*it)->to->vertex, to->vertex)) return *it;
                }
                it++;
            }
            return nullptr;
        }
        bool equals(VertexNode* node){
            //TODO
            if (vertexEQ == nullptr) {
                return this->vertex == node->vertex;
            } else {
                return vertexEQ(this->vertex, node->vertex);
            }
        }
        
        void removeTo(VertexNode* to){
            //TODO
            typename DLinkedList<Edge*>::Iterator it = adList.begin();
            while(it != adList.end()){
                if (vertexEQ == nullptr) {
                    if((*it)->to->vertex == to->vertex){
                        Edge* edge = *it;
                        adList.removeItem(edge);
                        this->outDegree_--;
                        to->inDegree_--;
                        delete edge;
                        break;
                    }
                } else {
                    if(vertexEQ((*it)->to->vertex, to->vertex)){
                        Edge* edge = *it;
                        adList.removeItem(edge);
                        this->outDegree_--;
                        to->inDegree_--;
                        delete edge;
                        break;
                    }
                }
                it++;
            }
        }
        int inDegree(){
            //TODO
            return inDegree_;
        }
        int outDegree(){
            //TODO
            return outDegree_;
        }
        string toString(){
            stringstream os;
            os << "V("
                    << this->vertex << ", "
                    << "in: " << this->inDegree_ << ", "
                    << "out: " << this->outDegree_ << ")";
            return os.str();
        }
    };
//END of VertexNode    
    
//BEGIN of Edge
    class Edge{
    private:
        VertexNode* from;
        VertexNode* to;
        float weight;
        friend class VertexNode;
        friend class AbstractGraph;
        
    public:
        Edge(){
            this->from = 0;
            this->to = 0;
            this->weight = 0;
        }
        Edge(VertexNode* from, VertexNode* to, float weight=0){
            this->from = from;
            this->to = to;
            this->weight = weight;
        }
        
        bool equals(Edge* edge){
            //TODO
            if (this->from->equals(edge->from) && this->to->equals(edge->to)) {
                return true;
            }
            return false;
           
        }

        static bool edgeEQ(Edge*& edge1, Edge*& edge2){
            return edge1->equals(edge2);
        }
        string toString(){
            stringstream os;
            os << "E("
                    << this->from->vertex
                    << ","
                    << this->to->vertex
                    << ","
                    << this->weight
                    << ")";
            return os.str();
        }
    };
 //END of Edge
    

//BEGIN of Iterator
public:
    class Iterator{
    private:
        typename DLinkedList<VertexNode*>::Iterator nodeIt;
        
    public:
        Iterator(AbstractGraph<T>* pGraph=0, bool begin=true){
            if(begin) {
                if(pGraph != 0) nodeIt = pGraph->nodeList.begin();
            }
            else{
                if(pGraph != 0) nodeIt = pGraph->nodeList.end();
            }
        }
        Iterator& operator=(const Iterator& iterator){
            this->nodeIt = iterator.nodeIt;
            return *this;
        }
        
        T& operator*(){
            return (*nodeIt)->vertex;
        }
        
        bool operator!=(const Iterator& iterator){
            return nodeIt != iterator.nodeIt;
        }
        // Prefix ++ overload 
        Iterator& operator++(){
            nodeIt++;
            return *this; 
        }
        // Postfix ++ overload 
        Iterator operator++(int){
            Iterator iterator = *this; 
            ++*this; 
            return iterator; 
        }
    };
//END of Iterator
};

#endif /* ABSTRACTGRAPH_H */

