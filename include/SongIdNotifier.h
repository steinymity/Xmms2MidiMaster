/*  Xmms2MidiMaster - XMMS2-Client emitting MIDI timecode to synchronize arbitrary MIDI-capable devices
 *  Copyright (C) 2014  Maximilian Stein
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SONGIDNOTIFIER_H_
#define _SONGIDNOTIFIER_H_

#include "typedefs.h"

/**
 * @brief   Build MIDI packages to send upon changes of the song id
 *          (song start and/or song stop) according to the configuration
 *
 * The class uses passed global song IDs (received from XMMS2) and maps them onto
 * MIDI commands to emit using the following steps:
 * - apply user settings: if a direct mapping exists use this one, otherwise add the
 *   offset
 * - use the specified MIDI command and channel
 * - use the lowest 14 bits only and clip the rest
 * - place them in the two data bytes in little or big endian
 *   => little endian means the lower significant bits are placed in the first data byte
 */
class SongIdNotifier
{
    public:
        /**
         * @brief   MIDI commands to send with this notifier
         *
         * Items correspond to the MIDI command codes.
         */
        enum ESongIdNotifierCommand
        {
            ESINC_NONE      = 0,    ///< don't send anything
            ESINC_NOTEOFF   = 0x80, ///< send NOTE OFF command
            ESINC_NOTEON    = 0x90, ///< send NOTE ON command
            ESINC_PA        = 0xA0, ///< send POLYPHONIC AFTERTOUCH command
            ESINC_CC        = 0xB0, ///< send CONTROL CHANGE command
        };

        /**
         * @brief   Constructor
         * @param   bCmd
         *              MIDI command to use for notifications
         * @param   bChannel
         *              MIDI channel [0..15] (physical channel)
         * @param   fLE
         *              Use little endian
         * @param   mpllId
         *              Map for direct mapping (overrides offset mapping)
         * @param   dlId
         *              Offset used if no direct mapping is available
         */
        SongIdNotifier(
               const IdMap&             mpllId,
               int                      dlId = 0,
               ESongIdNotifierCommand   bCmd = ESINC_NONE,
               MidiByte                 bChannel = 0,
               bool                     fLE = false
            );

        /**
         * @brief   Generate a MIDI message
         * @param   ilSongId
         *              The song id to send
         * @return  MIDI command to send:
         *              Byte 0: status byte
         *              Byte 1: data byte 1 (7 bits)
         *              Byte 2: data byte 2 (7 bits)
         *          Negative resulting song ids are represented in the two's complement.
         *          Byte order according to the set endianness See {@link setEndian(bool)} for details.
         *          If the MIDI command was set to ESINC_NONE, function returns 0.
         */
        MidiMsg getMsg( int ilSongId ) const;

        /**
         * @brief   Set the MIDI command
         * @param   bCmd
         *              Item of ESongIdNotifierCommand
         */
        void setMidiCommand( ESongIdNotifierCommand bCmd )
        {
            _rgbStatus = MIDI_STATUS_BYTE( bCmd, _rgbStatus );
        }

        /**
         * @brief   Set the MIDI channel
         * @param   bChannel
         *              MIDI channel as sent over the physical connection [0..15]
         */
        void setMidiChannel( MidiByte bChannel )
        {
            _rgbStatus = MIDI_STATUS_BYTE( _rgbStatus, bChannel );
        }

        /**
         * @brief   Set the transfer endian
         * @param   fLE
         *              True to use little endian, i.e. send the least significant bits first
         */
        void setEndian( bool fLE )
        {
            _fLE = fLE;
        }
        
        /**
         * @brief   Set the song id offset
         */
        void setSongIdOffset( int dlId )
        {
            _dlId = dlId;
        }


    private:
        const IdMap&        _mpllId;
        int                 _dlId;
        MidiByte            _rgbStatus; // MIDI status byte
        bool                _fLE; // use little endian
};

#endif // ifndef _SONGIDNOTIFIER_H_

