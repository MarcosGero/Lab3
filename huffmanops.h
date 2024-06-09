#ifndef HUFFMANOPS_H
#define HUFFMANOPS_H

// Calcula las frecuencias de los caracteres, y retorna dos arreglos: uno con los caracteres, y otro con sus respectivas frecuencias.
// Ademas, retorna el tama�o de esos arreglos
int calculateFrequencyTable(char* texto,size_t textSize, char* letras, int* frecuencias){
    int size = textSize;
    int freqTmp[256];

    // Prepara los arreglos
    memset(&freqTmp,0,256*sizeof(int));
    memset(frecuencias,0,256*sizeof(int));
    memset(letras,0,256);

    /*printf("\ntext: %s; size: %d\n",texto,size);*/

    // Escanea todo el archivo y va contando
    for(int i = 0; i < size; i++){
        freqTmp[(unsigned char) texto[i]]++;
    }

    int nonZeroIndex = 0; // Indice para ir llenando los arreglos letras y frecuencias
    // Muestra la tabla de las cantidades
    for(int i = 0; i < 256; i++){
        if (freqTmp[i] != 0){
//            printf("%c: %d\n",i,freqTmp[i]);
            frecuencias[nonZeroIndex] = freqTmp[i];
            letras[nonZeroIndex] = i;
            nonZeroIndex++;

        }
    }
    return nonZeroIndex;

}

// The main function that builds Huffman tree
MinHeapNode* buildHuffmanTree(char data[], int freq[], int size){
    MinHeapNode *left, *right, *top;

    // Step 1: Create a min heap of capacity
    // equal to size. Initially, there are
    // modes equal to size.
    MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

    // Iterate while size of heap doesn't become 1
    while (!isSizeOne(minHeap)) {

        // Step 2: Extract the two minimum
        // freq items from min heap
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        // Step 3: Create a new internal
        // node with frequency equal to the
        // sum of the two nodes frequencies.
        // Make the two extracted node as
        // left and right children of this new node.
        // Add this node to the min heap
        // '$' is a special value for internal nodes, not
        // used
        top = newNode('$', left->data.freq + right->data.freq);

        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }

    // Step 4: The remaining node is the
    // root node and the tree is complete.
    return extractMin(minHeap);
}


void freeHuffmanTree(MinHeapNode* root){
    if (root == NULL) return;

    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    free(root);
}

// The main function that builds a
// Huffman Tree and print codes by traversing
// the built Huffman Tree
MinHeapNode* HuffmanCodes(char data[], int freq[], int size){
    // Construct Huffman Tree
    MinHeapNode* root = buildHuffmanTree(data, freq, size);

    // Print Huffman codes using
    // the Huffman tree built above
    int arr[MAX_TREE_HT], top = 0;

    /*printCodes(root, arr, top);*/

    return root;
}

/* UTILITY FUNCTIONS */
/* Function to find index of the maximum value in arr[start...end] */

int maximo (huffmanData arr[], int strt, int end)
{
    int i, max = arr[strt].freq, maxind = strt;

    for(i = strt+1; i <= end; i++)
    {
        if(arr[i].freq > max)
        {
            max = arr[i].freq;
            maxind = i;
        }
    }

    return maxind;
}


/* Recursive function to construct binary of size len from
   Inorder traversal inorder[]. Initial values of start and end
   should be 0 and len -1.  */
MinHeapNode* buildTreeFromInorder (huffmanData inorder[], int start, int end)
{
    if (start > end)
        return NULL;

    /* Find index of the maximum element from Binary Tree */
    int i = maximo(inorder, start, end);

    /* Pick the maximum value and make it root */
    MinHeapNode *root = newNode(inorder[i].c,inorder[i].freq);

    /* If this is the only element in inorder[start..end],

       then return it */
    if (start == end)
        return root;

    /* Using index in Inorder traversal, construct left and
       right subtress */

    root->left  = buildTreeFromInorder (inorder, start, i-1);
    root->right = buildTreeFromInorder (inorder, i+1, end);
    return root;
}

// Convierte un arbol de huffman en un arreglo de char (la frecuencia ya no se necesitará) con barrido inorder
void createInorderTree(MinHeapNode* root, huffmanData* inorder,int* size){

    // Caso base
    if (root == NULL){
        return;
    }

    // Recorrido recursivo inorder
    createInorderTree(root->left,inorder,size);
    inorder[*size] = root->data;
    (*size)++;
    createInorderTree(root->right,inorder,size);
}


void createHuffmanTable(MinHeapNode* root,char arr[],int top,char* table){
    // Nota: el recorrido es post-order

    // Assign 0 to left edge and recur
    if (root->left) {
        arr[top] = '0';
        createHuffmanTable(root->left, arr, top + 1,table);
    }

    // Assign 1 to right edge and recur
    if (root->right) {
        arr[top] = '1';
        createHuffmanTable(root->right, arr, top + 1,table);
    }

    if (isLeaf(root)) {
//        printf("%c: %s\n",root->data.c,arr);
        // Caso en donde el arbol de huffman es solo de un nodo
        if (top == 0){
            arr[top] = '0';
            top++;
        }
        arr[top+1] = '\0'; // Para convertirlo en string
        strncpy(table+root->data.c*MAX_TREE_HT,arr,top);
    }
}
#endif // HUFFMANOPS_H
