
#define MAX_TREE_HT 100

// Par caracter,probabilidad
typedef struct{
    char c;
    int freq;
}huffmanData;

// Nodo del arbol para Huffman
typedef struct MinHeapNode MinHeapNode;
struct MinHeapNode{
    // El par (char, int)
    huffmanData data;

    // Puntero a Hijo izquierdo y derecho
    MinHeapNode* left;
    MinHeapNode* right;
};

// A Min Heap: Collection of
// min-heap (or Huffman tree) nodes
typedef struct{

    // Current size of min heap
    unsigned size;

    // capacity of min heap
    unsigned capacity;

    // Array of minheap node pointers
    MinHeapNode** array;
}MinHeap;

// A utility function allocate a new
// min heap node with given character
// and frequency of the character
MinHeapNode* newNode(char data, unsigned freq)
{
    MinHeapNode* temp = (MinHeapNode*)malloc(sizeof(MinHeapNode));

    temp->left = temp->right = NULL;
    temp->data.c = data;
    temp->data.freq = freq;

    return temp;
}

// A utility function to create
// a min heap of given capacity
MinHeap* createMinHeap(unsigned capacity){
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));

    // current size is 0
    minHeap->size = 0;

    minHeap->capacity = capacity;

    minHeap->array = (MinHeapNode**)malloc(minHeap->capacity * sizeof(MinHeapNode*));
    return minHeap;
}

// A utility function to
// swap two min heap nodes
void swapMinHeapNode(MinHeapNode** a,MinHeapNode** b){
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// The standard minHeapify function.
void minHeapify(MinHeap* minHeap, int idx){
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size
        && minHeap->array[left]->data.freq
            < minHeap->array[smallest]->data.freq)
        smallest = left;

    if (right < minHeap->size
        && minHeap->array[right]->data.freq
            < minHeap->array[smallest]->data.freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest],
                        &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// A utility function to check
// if size of heap is 1 or not
int isSizeOne(MinHeap* minHeap){
    return (minHeap->size == 1);
}

// A standard function to extract
// minimum value node from heap
MinHeapNode* extractMin(MinHeap* minHeap){
    MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];

    --minHeap->size;
    minHeapify(minHeap, 0);

    return temp;
}

// A utility function to insert
// a new node to Min Heap
void insertMinHeap(MinHeap* minHeap,MinHeapNode* minHeapNode){
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i
        && minHeapNode->data.freq
                < minHeap->array[(i - 1) / 2]->data.freq) {

        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }

    minHeap->array[i] = minHeapNode;
}

// A standard function to build min heap
void buildMinHeap(MinHeap* minHeap){
    int n = minHeap->size - 1;
    int i;

    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

MinHeap* createAndBuildMinHeap(char data[],int freq[], int size){

    MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);

    return minHeap;
}

// A utility function to print an array of size n
void printArr(int arr[], int n)
{
    int i;
    for (i = 0; i < n; ++i)
        printf("%d", arr[i]);

    printf("\n");
}

// Utility function to check if this node is leaf
int isLeaf(MinHeapNode* root)
{
    return !(root->left) && !(root->right);
}
