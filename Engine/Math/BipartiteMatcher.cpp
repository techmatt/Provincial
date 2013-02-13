#include "Main.h"


void BipartiteMatcherGreedyPriority::match(const Grid<float> &m, UINT rows, UINT cols, Vector<UINT> &result)
{
    if(result.Length() < rows)
    {
        result.Allocate(rows);
    }

    const UINT finalMatchCount = Math::Max(rows, cols);
    
    Vector<bool> matched1(rows, false);
    Vector<bool> matched2(cols, false);

    // Construct the heap and prepare the output matrix
    priority_queue<BipartiteHelper::Match, vector<BipartiteHelper::Match>, BipartiteHelper::MatchCompRev> heap;
    for (UINT i = 0; i < rows; i++)
    {
        for (UINT j = 0; j < cols; j++)
        {
            heap.push(BipartiteHelper::Match(i, j, m(i,j) ));
        }
    }

    // Just successively pop everything, always taking the match
    // if it's legal
    UINT matchCount = 0;
    while (!heap.empty())
    {
        const BipartiteHelper::Match& match = heap.top();
        if (!matched1[match.i] && !matched2[match.j])
        {
            matched1[match.i] = true;
            matched2[match.j] = true;
            result[match.i] = match.j;
            matchCount++;
            if(matchCount == finalMatchCount)
            {
                return;
            }
        }
        heap.pop();
    }
}


void BipartiteMatcherGreedyVector::match(const Grid<float> &m, UINT rows, UINT cols, Vector<UINT> &result)
{
    if(result.Length() < rows)
    {
        result.Allocate(rows);
    }

    const UINT finalMatchCount = Math::Max(rows, cols);
    const UINT heapSize = rows * cols;
    
    Vector<bool> matched1(rows, false);
    Vector<bool> matched2(cols, false);

    Vector<BipartiteHelper::Match> heap(heapSize);

    UINT heapIndexA = 0;
    for (UINT i = 0; i < rows; i++)
    {
        for (UINT j = 0; j < cols; j++)
        {
            heap[heapIndexA++] = BipartiteHelper::Match(i, j, m(i, j));
        }
    }

    BipartiteHelper::MatchComp comparison;
    heap.Sort(comparison);

    UINT matchCount = 0;
    UINT heapIndexB = 0;
    while (heapIndexB < heapSize)
    {
        const BipartiteHelper::Match& match = heap[heapIndexB++];
        if (!matched1[match.i] && !matched2[match.j])
        {
            matched1[match.i] = true;
            matched2[match.j] = true;
            result[match.i] = match.j;
            matchCount++;
            if(matchCount == finalMatchCount)
            {
                return;
            }
        }
    }
}


void BipartiteMatcherUpperBound::match(const Grid<float> &m, UINT rows, UINT cols, Vector<UINT> &result)
{
    if(result.Length() < rows)
    {
        result.Allocate(rows);
    }

    const UINT rowCount = rows;
    const UINT colCount = cols;
    
    for (UINT i = 0; i < rowCount; i++)
    {
        UINT bestColumn = 0;
        float bestValue = m(i, 0);
        for (UINT j = 1; j < colCount; j++)
        {
            float curValue = m(i, j);
            if(curValue < bestValue)
            {
                bestValue = curValue;
                bestColumn = j;
            }
        }
        result[i] = bestColumn;
    }
}


void BipartiteMatcherHungarianKnuth::match(const Grid<float> &m, UINT rows, UINT cols, Vector<UINT> &result)
{
    if(result.Length() < rows)
    {
        result.Allocate(rows);
    }

    initArrays(m, rows, cols);

    hungarian(result);

    deinitArrays();
}


void BipartiteMatcherHungarianKnuth::initArrays(const Grid<float> &m, UINT rows, UINT cols)
{
    size1 = rows;
    size2 = cols;

    // Alloc Array
    Array = new float*[size1];
    for (int i = 0; i < size1; i++)
    {
        Array[i] = new float[size2];
    }

    // Initialize Array
    for (int i = 0; i < size1; i++) for (int j = 0; j < size2; j++)
    {
        Array[i][j] = m(i,j);
    }
}


void BipartiteMatcherHungarianKnuth::deinitArrays()
{
    for (int i = 0; i < size1; i++)
    {
        delete[] Array[i];
    }
    delete[] Array;
}


void BipartiteMatcherHungarianKnuth::hungarian(Vector<UINT> &result)
{
    const float infinity = std::numeric_limits<float>::max();
    const UINT verbose = 0;

    int j;
    //int false=0,true=1;

    int m=size1,n=size2;
    int k;
    int l;
    float s;
    //int col_mate[size1]={0};
    //int row_mate[size2]={0};
    //int parent_row[size2]={0};
    //int unchosen_row[size1]={0};
    Vector<int> col_mate; col_mate.Allocate(size1, 0);
    Vector<int> row_mate; row_mate.Allocate(size2, 0);
    Vector<int> parent_row; parent_row.Allocate(size2, 0);
    Vector<int> unchosen_row; unchosen_row.Allocate(size1, 0);
    int t;
    int q;
    //float row_dec[size1]={0};
    //float col_inc[size2]={0};
    //float slack[size2]={0};
    //int slack_row[size2]={0};
    Vector<float> row_dec; row_dec.Allocate(size1, 0.0f);
    Vector<float> col_inc; col_inc.Allocate(size2, 0.0f);
    Vector<float> slack; slack.Allocate(size2, 0.0f);
    Vector<int> slack_row; slack_row.Allocate(size2, 0);
    int unmatched;
    float cost=0;

    // Begin subtract column minima in order to start with lots of zeroes 12
    //printf("Using heuristic\n");
    for (l=0;l<n;l++)
    {
        s=Array[0][l];
        for (k=1;k<n;k++)
            if (Array[k][l]<s)
                s=Array[k][l];
        cost+=s;
        if (s!=0)
            for (k=0;k<n;k++)
                Array[k][l]-=s;
    }
    // End subtract column minima in order to start with lots of zeroes 12

    // Begin initial state 16
    t=0;
    for (l=0;l<n;l++)
    {
        row_mate[l]= -1;
        parent_row[l]= -1;
        col_inc[l]=0;
        slack[l]=infinity;
    }
    for (k=0;k<m;k++)
    {
        s=Array[k][0];
        for (l=1;l<n;l++)
            if (Array[k][l]<s)
                s=Array[k][l];
        row_dec[k]=s;
        for (l=0;l<n;l++)
            if (s==Array[k][l] && row_mate[l]<0)
            {
                col_mate[k]=l;
                row_mate[l]=k;
                if (verbose)
                    printf("matching col %d==row %d\n",l,k);
                goto row_done;
            }
            col_mate[k]= -1;
            if (verbose)
                printf("node %d: unmatched row %d\n",t,k);
            unchosen_row[t++]=k;
row_done:
            ;
    }
    // End initial state 16

    // Begin Hungarian algorithm 18
    if (t==0)
        goto done;
    unmatched=t;
    while (1)
    {
        if (verbose)
            printf("Matched %d rows.\n",m-t);
        q=0;
        while (1)
        {
            while (q<t)
            {
                // Begin explore node q of the forest 19
                {
                    k=unchosen_row[q];
                    s=row_dec[k];
                    for (l=0;l<n;l++)
                        if (slack[l])
                        {
                            float del;
                            del=Array[k][l]-s+col_inc[l];
                            if (del<slack[l])
                            {
                                if (del==0)
                                {
                                    if (row_mate[l]<0)
                                        goto breakthru;
                                    slack[l]=0;
                                    parent_row[l]=k;
                                    if (verbose)
                                        printf("node %d: row %d==col %d--row %d\n",
                                        t,row_mate[l],l,k);
                                    unchosen_row[t++]=row_mate[l];
                                }
                                else
                                {
                                    slack[l]=del;
                                    slack_row[l]=k;
                                }
                            }
                        }
                }
                // End explore node q of the forest 19
                q++;
            }

            // Begin introduce a new zero into the matrix 21
            s=infinity;
            for (l=0;l<n;l++)
                if (slack[l] && slack[l]<s)
                    s=slack[l];
            for (q=0;q<t;q++)
                row_dec[unchosen_row[q]]+=s;
            for (l=0;l<n;l++)
                if (slack[l])
                {
                    slack[l]-=s;
                    if (slack[l]==0)
                    {
                        // Begin look at a new zero 22
                        k=slack_row[l];
                        if (verbose)
                            printf(
                            "Decreasing uncovered elements by %d produces zero at [%d,%d]\n",
                            s,k,l);
                        if (row_mate[l]<0)
                        {
                            for (j=l+1;j<n;j++)
                                if (slack[j]==0)
                                    col_inc[j]+=s;
                            goto breakthru;
                        }
                        else
                        {
                            parent_row[l]=k;
                            if (verbose)
                                printf("node %d: row %d==col %d--row %d\n",t,row_mate[l],l,k);
                            unchosen_row[t++]=row_mate[l];
                        }
                        // End look at a new zero 22
                    }
                }
                else
                    col_inc[l]+=s;
            // End introduce a new zero into the matrix 21
        }
breakthru:
        // Begin update the matching 20
        if (verbose)
            printf("Breakthrough at node %d of %d!\n",q,t);
        while (1)
        {
            j=col_mate[k];
            col_mate[k]=l;
            row_mate[l]=k;
            if (verbose)
                printf("rematching col %d==row %d\n",l,k);
            if (j<0)
                break;
            k=parent_row[j];
            l=j;
        }
        // End update the matching 20
        if (--unmatched==0)
            goto done;
        // Begin get ready for another stage 17
        t=0;
        for (l=0;l<n;l++)
        {
            parent_row[l]= -1;
            slack[l]=infinity;
        }
        for (k=0;k<m;k++)
            if (col_mate[k]<0)
            {
                if (verbose)
                    printf("node %d: unmatched row %d\n",t,k);
                unchosen_row[t++]=k;
            }
            // End get ready for another stage 17
    }
done:

    const bool checkingResult = false;
    if(checkingResult)
    {
        // Begin doublecheck the solution 23
        for (k=0;k<m;k++)
            for (l=0;l<n;l++)
                if (Array[k][l]<row_dec[k]-col_inc[l])
                    SignalError("Hungarian failure");
        for (k=0;k<m;k++)
        {
            l=col_mate[k];
            if (l<0 || Array[k][l]!=row_dec[k]-col_inc[l])
                SignalError("Hungarian failure");
        }
        k=0;
        for (l=0;l<n;l++)
            if (col_inc[l])
                k++;
        if (k>m)
            SignalError("Hungarian failure");
        // End doublecheck the solution 23
        // End Hungarian algorithm 18
    }

    for(UINT resultIndex = 0; resultIndex < UINT(m); resultIndex++)
    {
        result[resultIndex] = col_mate[resultIndex];
    }
}
