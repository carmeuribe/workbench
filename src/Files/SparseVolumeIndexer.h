#ifndef __SPARSE_VOLUME_INDEXER_H__
#define __SPARSE_VOLUME_INDEXER_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "CaretObject.h"


namespace caret {
    class CiftiInterface;
    
    class SparseVolumeIndexer : public CaretObject {
        
    public:
        /**
         * Location of voxel data.
         */
        enum ColumnOrRow {
            /** Use voxel data for columns */
            COLUMN,
            /** Use voxel data for rows */
            ROW
        };
        
        SparseVolumeIndexer(const CiftiInterface* ciftiInterface,
                            const ColumnOrRow columnOrRow);
        
        virtual ~SparseVolumeIndexer();
        
        int64_t getOffsetForIndices(const int64_t i,
                                    const int64_t j,
                                    const int64_t k) const;
        
        int64_t getOffsetForCoordinate(const float x,
                                       const float y,
                                       const float z) const;
        
    private:
        SparseVolumeIndexer(const SparseVolumeIndexer&);

        SparseVolumeIndexer& operator=(const SparseVolumeIndexer&);
        
    public:

        // ADD_NEW_METHODS_HERE


    private:
        // ADD_NEW_MEMBERS_HERE

        bool m_dataValid;
        
        int64_t m_dimI;
        
        int64_t m_dimJ;
        
        int64_t m_dimK;
        
        float m_spacingX;
        
        float m_spacingY;
        
        float m_spacingZ;
        
        float m_originX;
        
        float m_originY;
        
        float m_originZ;
        
        std::vector<int32_t> m_voxelOffsets;
        
    };
    
#ifdef __SPARSE_VOLUME_INDEXER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SPARSE_VOLUME_INDEXER_DECLARE__

} // namespace
#endif  //__SPARSE_VOLUME_INDEXER_H__
