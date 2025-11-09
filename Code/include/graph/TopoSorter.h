/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TopoSorter.h
 * Author: ltsach
 *
 * Created on July 11, 2021, 10:21 PM
 */

#ifndef TOPOSORTER_H
#define TOPOSORTER_H
#include "graph/DGraphModel.h"
#include "list/DLinkedList.h"
#include "sorting/DLinkedListSE.h"
#include "hash/xMap.h"
#include "stacknqueue/Queue.h"
#include "stacknqueue/Stack.h"

template<class T>
class TopoSorter{
public:
    static int DFS;
    static int BFS; 
    
protected:
    DGraphModel<T>* graph;
    int (*hash_code)(T&, int);
    
public:
    TopoSorter(DGraphModel<T>* graph, int (*hash_code)(T&, int)=0){
        this->graph = graph;
        this->hash_code = hash_code;
    }   
    DLinkedList<T> sort(int mode=DFS, bool sorted=true){
        if (mode == DFS) {
            return dfsSort(sorted);
        } else {
            return bfsSort(sorted);
        }
    }
    DLinkedList<T> bfsSort(bool sorted = true) {

    xMap<T, int> inDegrees = vertex2inDegree(hash_code); 
    Queue<T> Queue;//zeroInDegreeQueue
    DLinkedList<T> zeroInDegreeNodes = listOfZeroInDegrees();

     if (sorted) {
        DLinkedListSE zeroInDegreeNodesSE(zeroInDegreeNodes);
        zeroInDegreeNodesSE.sort();
        DLinkedList<T> tmp;
        for (typename DLinkedList<T>::Iterator it = zeroInDegreeNodesSE.begin(); it != zeroInDegreeNodesSE.end(); ++it) {
            tmp.add(*it);
        }
        zeroInDegreeNodes = tmp;
    }
    for (typename DLinkedList<T>::Iterator it = zeroInDegreeNodes.begin(); it != zeroInDegreeNodes.end(); ++it) {
        Queue.push(*it);
    }
    DLinkedList<T> result;

    while (!Queue.empty()) { 
        T current = Queue.pop(); 
        result.add(current);            

        DLinkedList<T> neighbors = graph->getOutwardEdges(current);
        for (typename DLinkedList<T>::Iterator it = neighbors.begin(); it != neighbors.end(); ++it) {
            T neighbor = *it;
            int degree = inDegrees.containsKey(neighbor) ? inDegrees.get(neighbor) : 0;
            degree -= 1;
            inDegrees.put(neighbor, degree); 
            if (degree == 0) Queue.push(neighbor);
        }
    }
    
    return result;
    }

    void dfs(T u, xMap<T, bool>& visited, DLinkedList<T>& result){
        visited.put(u, true);
        DLinkedList<T> neighbors = graph->getOutwardEdges(u);
        for (typename DLinkedList<T>::Iterator it = neighbors.begin(); it != neighbors.end(); ++it) {
            T v = *it;
            if (!visited.containsKey(v)) {
                dfs(v, visited, result);
            }
        }
        result.add(u);
    }

    DLinkedList<T> dfsSort(bool sorted=true){
        DLinkedList<T> result;
        xMap<T, bool> visited(this->hash_code);
        DLinkedList<T> vertices= graph->vertices();
        DLinkedList<T> zeroInDegrees = this->listOfZeroInDegrees();

        if (sorted) {
            DLinkedListSE zeroInDegreesSE(zeroInDegrees);
            zeroInDegreesSE.sort();
            DLinkedList<T> tmp;
            for (typename DLinkedList<T>::Iterator it = zeroInDegreesSE.begin(); it != zeroInDegreesSE.end(); ++it) {
                tmp.add(*it);
            }
            zeroInDegrees = tmp;
        }

        for (typename DLinkedList<T>::Iterator it = zeroInDegrees.begin(); it != zeroInDegrees.end(); ++it) {
            dfs(*it, visited, result);
        }

        DLinkedList<T> tmp;
            for (typename DLinkedList<T>::BWDIterator it = result.bbegin(); it != result.bend(); --it) {
                tmp.add(*it);
            }
            result = tmp;

        // if (sorted) {
        //     DLinkedList<T> tmp;
        //     for (typename DLinkedList<T>::BWDIterator it = result.bbegin(); it != result.bend(); --it) {
        //         tmp.add(*it);
        //     }
        //     result = tmp;
        // }
        
        return result;
    }

protected:

    xMap<T, int> vertex2inDegree(int (*hash)(T&, int)) {
        xMap<T, int> inDegree(hash);
        DLinkedList<T> vertices = graph->vertices();

        for (int i=0; i<vertices.size(); i++) {
            inDegree.put(vertices.get(i), graph->inDegree(vertices.get(i)));
        }

        return inDegree;
    }

    xMap<T, int> vertex2outDegree(int (*hash)(T&, int)) {
        xMap<T, int> outDegree(hash);
        DLinkedList<T> vertices = graph->vertices();

        for (int i=0; i<vertices.size(); i++) {
            outDegree.put(vertices.get(i), graph->outDegree(vertices.get(i)));
        }

        return outDegree;
    }

    DLinkedList<T> listOfZeroInDegrees() {
        DLinkedList<T> zeroInDegrees;
        DLinkedList<T> vertices = graph->vertices();

        for (typename DLinkedList<T>::Iterator it = vertices.begin(); it != vertices.end(); ++it) {
            if (graph->inDegree(*it) == 0) {
                zeroInDegrees.add(*it);
            }
        }
        return zeroInDegrees;
    }
}; //TopoSorter

template<class T>
int TopoSorter<T>::DFS = 0;

template<class T>
int TopoSorter<T>::BFS = 1;

/////////////////////////////End of TopoSorter//////////////////////////////////


#endif /* TOPOSORTER_H */

