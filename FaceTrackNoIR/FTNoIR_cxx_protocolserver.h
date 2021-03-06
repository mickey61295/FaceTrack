/********************************************************************************
* FTNoIR_cxx_protocolserver is the base Class for Game-protocol-servers.		*
*						    Using this, the tracker only needs to create one	*
*						    server and can use the same functions to			*
*							communicate with it.								*
*																				*
* Copyright (C) 2010	Wim Vriend (Developing)									*
*						Ron Hendriks (Testing and Research)						*
*																				*
* This program is free software; you can redistribute it and/or modify it		*
* under the terms of the GNU General Public License as published by the			*
* Free Software Foundation; either version 3 of the License, or (at your		*
* option) any later version.													*
*																				*
* This program is distributed in the hope that it will be useful, but			*
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY	*
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for	*
* more details.																	*
*																				*
* You should have received a copy of the GNU General Public License along		*
* with this program; if not, see <http://www.gnu.org/licenses/>.				*
*																				*
********************************************************************************/
#ifndef FTNOIR_CXX_PROTOCOLSERVER_H
#define FTNOIR_CXX_PROTOCOLSERVER_H

#include <windows.h> 
//#include <QThread>
#include <QObject>

namespace v4friend
{
    namespace ftnoir
    {
		class ProtocolServerBase : public QObject // : public QThread
        {
		Q_OBJECT
        public:

			virtual ~ProtocolServerBase() {}

			//! @return true if the engine will produce data.
            /*! @see smEngineIsLicensed() */
			virtual bool isLicensed() const { return true; };
			virtual	QString GetProgramName() { return QString("Test"); };

			virtual bool checkServerInstallationOK ( HANDLE handle ) { return true; }
			virtual void sendHeadposeToGame() {}
			virtual void setVirtRotX(float rot) { virtRotX = rot; }
			virtual void setVirtRotY(float rot) { virtRotY = rot; }
			virtual void setVirtRotZ(float rot) { virtRotZ = rot; }
			virtual void setVirtPosX(float pos) { virtPosX = pos / 100.0f; }
			virtual void setVirtPosY(float pos) { virtPosY = pos / 100.0f; }
			virtual void setVirtPosZ(float pos) { virtPosZ = pos / 100.0f; }

			virtual void setHeadRotX(float x) { headRotX = x; }
			virtual void setHeadRotY(float y) { headRotY = y; }
			virtual void setHeadRotZ(float z) { headRotZ = z; }
			virtual void setHeadPosX(float x) { headPosX = x; }
			virtual void setHeadPosY(float y) { headPosY = y; }
			virtual void setHeadPosZ(float z) { headPosZ = z; }

			virtual void resetProperties() { 
				virtPosX = 0.0f;
				virtPosY = 0.0f;
				virtPosZ = 0.0f;
				virtRotX = 0.0f;
				virtRotY = 0.0f;
				virtRotZ = 0.0f;

				headPosX = 0.0f;
				headPosY = 0.0f;
				headPosZ = 0.0f;
				headRotX = 0.0f;
				headRotY = 0.0f;
				headRotZ = 0.0f;

				prevPosX = 0.0f;
				prevPosY = 0.0f;
				prevPosZ = 0.0f;
				prevRotX = 0.0f;
				prevRotY = 0.0f;
				prevRotZ = 0.0f;
			}

        protected:
			ProtocolServerBase() {};

		public:
			/** member variables for saving the head pose **/
			float virtPosX;
			float virtPosY;
			float virtPosZ;
	
			float virtRotX;
			float virtRotY;
			float virtRotZ;

			float headPosX;
			float headPosY;
			float headPosZ;
	
			float headRotX;
			float headRotY;
			float headRotZ;

			float prevPosX;
			float prevPosY;
			float prevPosZ;
			float prevRotX;
			float prevRotY;
			float prevRotZ;

			//
			// Values for analysis of tracking...
			//
			float confidence;
			float smoothvalue;
			float prev_value;
			bool newSample;
			float dT;

		private:
            ProtocolServerBase(const ProtocolServerBase &);
            ProtocolServerBase &operator=(const ProtocolServerBase &);
            bool operator==(const ProtocolServerBase &) const;
        };
    }
}
#endif
