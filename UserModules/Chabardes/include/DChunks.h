#ifndef _DCHUNKS_H_
#define _DCHUNKS_H_

#include "DIOStream.h"

namespace smil {

    template <class T>
    class ChunkArray {
        public:
            ChunkArray () {}
            ~ChunkArray () {
                if (rawData != NULL)
                    ::operator delete (rawData);
                if (availability != NULL)
                    delete[] availability;
            }
            RES_T initialize (const int &_size, const IOStream &is) {
                size = _size;
                int max_dimension = is.get_chunks_len (0);
                for (int i=1; i<is.get_nbr_diff_chunks(); ++i)
                    max_dimension = MAX (max_dimension, is.get_chunks_len(i));
                memory_step = 6 * sizeof (int) + max_dimension * sizeof (T);
                rawData = ::operator new (size*memory_step);
                availability = new bool [size];
                if (rawData == NULL || availability == NULL )
                    return RES_ERR_BAD_ALLOCATION;
                for (int i=0; i<size; ++i) availability[i] = true;

                return RES_OK;
            }
            RES_T send (const IOStream &is) {
                //Scan for first unavailable
                int i=0;
                while (i<size && availability[i]) {
                    i++;
                }
                //Send
                //MPI_Send (rawData+i*memory_step, 1, is.mpi_types);
                //Mark it available 
            }
            RES_T next (const IOStream &is) {
                //Scan for first available
                //Mark it unavailable
                //Copy data from is
            }
            RES_T scatter (const IOStream &is) {
                //Check if all unavailable
                //Send all
                //Mark everything available
            }
            RES_T nextRound (const IOStream &is) {
                //Check if all available
                //Mark all unavailable
                //Copy data from is
            }
        private:
            void* rawData;
            bool* availability;
            int memory_step;
            int size;
    };

    template <class T>
    class Chunk {
        public:
            Chunk () : is_initialized (false) { }
            ~Chunk () {
            }
            RES_T setMemorySpace (void* ptr, const int _sent_size) {
                sent_size = _sent_size;
                if (ptr == NULL) { return RES_ERR_BAD_ALLOCATION; }
                is_initialized = true; 
                size = (int*)ptr;
                offset = (int*)ptr + 3;
                data = (T*)((int*)ptr + 6);
            }
            RES_T createFromArray (
                    const T* dataIn,
                    const int &sd_x, 
                    const int &sd_y,
                    const int &o_x,
                    const int &o_y,
                    const int &o_z,
                    const int &sc_x,
                    const int &sc_y,
                    const int &sc_z) {
                ASSERT (is_initialized);
                size[0] = sc_x; size[1] = sc_y; size[2] = sc_z;
                offset[0] = o_x; offset[1] = o_y; offset[2] = o_z; 

                for (int k=0; k<sc_z; ++k) {
                    for (int j=0; j<sc_y; ++j) { 
                        if (memcpy (data + j * sc_x + k * sc_x * sc_y, 
                                    dataIn + o_x + (o_y+j) * sd_x + (o_z+k) * sd_x * sd_y,
                                    sc_x*sizeof(T)) == NULL){
                            return RES_ERR_IO;
                        }
                    }
                }
                return RES_OK;
                   
            }
            RES_T storeToArray (const int &sd_x, const int &sd_y, T* dataOut) {
                for (int k=0; k<size[2]; ++k) {
                    for (int j=0; j<size[1]; ++j) {
                        if (memcpy (dataOut + offset[0] + (offset[1]+j) * sd_x + (offset[2]+k) * sd_x * sd_y,
                                    data + j * size[0] + k * size[0] * size[1], 
                                    size[0]*sizeof(T)) == NULL){
                            return RES_ERR_IO;
                        }
                    }
                }
                return RES_OK;
            } 
            RES_T storeToImage (Image<T> &imOut) {
//              ASSERT (all the good stuff).

                size_t s[3];
                imOut.getSize(s);

                if (storeToArray (s[0], s[1], imOut.getPixels()) != RES_OK) return RES_ERR;
                return RES_OK;
            }
            int getSize (const unsigned int &dimension) {
                ASSERT (dimension < 3) ;
                return size[dimension] ;
            }
            int getOffset (const unsigned int &dimension) {
                ASSERT (dimension < 3) ;
                return offset[dimension] ;

            }
            T* getData () {
                return data;
            }
            void print (bool print_data = false) {
                cout << "#### size : [" << size[0] << ", " << size[1] << ", " << size[2] << "], offset : [" << offset[0] << ", " << offset[1] << ", " << offset[2] << "] ####" << endl;
                if (!print_data) return;
                for (int i=0; i<size[2]; ++i) {
                    for (int j=0; j<size[1]; ++j) {
                        for (int k=0; k<size[0]; ++k)
                            cout << (int) data[k+j*size[0]+i*size[0]*size[1]] << ' ';
                        cout << endl;
                    }
                    cout << endl;
                }
                cout << endl; 
            }
        private:
            bool is_initialized;
            int sent_size;
            int* size;
            int* offset;
            T* data;
    };
}

#endif