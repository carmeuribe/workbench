#ifndef __EVENT_SPEC_FILE_READ_DATA_FILES_H__
#define __EVENT_SPEC_FILE_READ_DATA_FILES_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include "DataFileTypeEnum.h"
#include "Event.h"

namespace caret {

    class Brain;
    class SpecFile;
    
    /**
     * Event for reading data files selected in a spec file.
     */
    class EventSpecFileReadDataFiles : public Event {
        
    public:
        EventSpecFileReadDataFiles(Brain* loadIntoBrain,
                                   SpecFile* specFile);
        
        virtual ~EventSpecFileReadDataFiles();
        
        SpecFile* getSpecFile();
        
        Brain* getLoadIntoBrain();
        
        AString getUsername() const;
        
        AString getPassword() const;
        
        void setUsernameAndPassword(const AString& username,
                                    const AString& password);
        
    private:
        EventSpecFileReadDataFiles(const EventSpecFileReadDataFiles&);
        
        EventSpecFileReadDataFiles& operator=(const EventSpecFileReadDataFiles&);
        
        Brain* loadIntoBrain;
        
        SpecFile* specFile;

        AString username;
        
        AString password;
        
        bool errorInvalidStructure;
    };

} // namespace

#endif // __EVENT_SPEC_FILE_READ_DATA_FILES_H__
