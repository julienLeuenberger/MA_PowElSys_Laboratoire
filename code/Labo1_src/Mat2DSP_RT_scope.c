//
// copyright 2012 by
// Raoul Herzog
// Institute for Industrial Automation (iAi)
// HEIG-VD, University of Applied Sciences Western Switzerland
// CH-1401 YVERDON
// Switzerland
//

#define MAX_SCOP_VAR        10
#define MAX_SCOP_BUF_DUMMY   1

#pragma DATA_SECTION(_ScBuf, "SCOPBUF"); // linked at the end of RAML4567
unsigned int   _ScBuf[MAX_SCOP_BUF_DUMMY];

unsigned int  *_ScVarList[MAX_SCOP_VAR+1];
unsigned int   _ScWSizList[MAX_SCOP_VAR];
unsigned int   _ScTimScl, _ScTimCtr, _ScTrigStat, _ScPreTrCtr, _ScPstTrCtr;
unsigned int  *_ScBufEnd, *_ScBufPtr;
int            _ScTrigLev, *_ScTrigVar;

void IniScope(void)  {
	_ScPstTrCtr = 0;
	_ScPreTrCtr = 0;
	_ScTrigStat = 0;
	_ScBufPtr   = (unsigned int*) &_ScBuf; // evtl. not necessary
}

inline void Copy2Buf(void) {
	unsigned int k = 0, l, *Ptr;
	
	while (_ScVarList[k] != 0) {                     // till end of _ScVarList
		l   = _ScWSizList[k];
		Ptr = _ScVarList[k];
		while (l-- > 0) {
			*_ScBufPtr++ = *Ptr++;
		}
		k++;
	}
    if (_ScBufPtr > _ScBufEnd) {                     // _ScBufEnd calculated in Matlab in order to fit multiple
		_ScBufPtr = (unsigned int*) &_ScBuf;         // wrap around
	}
}

inline int Trigger(void) {
	int diff;
	if (_ScTrigStat == 0) {
		return(1); 
		}
	else {
		diff = *_ScTrigVar - _ScTrigLev;
		if ((_ScTrigStat & 0x18) != 0) { 
			if (diff > 0) {                              /* arming      */
				_ScTrigStat = _ScTrigStat & 0xf;           /* clear bit 4 */	
			}
			if (diff < 0) {
				_ScTrigStat = _ScTrigStat & 0x17;          /* clear bit 3 */	
			}
		}
		else {
			if (diff > 0) {
				_ScTrigStat = _ScTrigStat & 0x5;           /* clear bit 1 */
			}
			if (diff < 0) {
				_ScTrigStat = _ScTrigStat & 0x3;           /* clear bit 2 */
			}
			if (diff == 0) {
				_ScTrigStat = _ScTrigStat & 0x6;           /* clear bit 0 */
			}	
		}
	return(0);
	}
}

void RTScope(void) {
	if (_ScPstTrCtr != 0) {
		if (_ScPreTrCtr != 0) {
			if (_ScTimCtr != 0) {
					_ScTimCtr--;
			}
			else {
				_ScTimCtr = _ScTimScl;
				Copy2Buf();
				_ScPreTrCtr--;				
			}
		}
		else {
			if (_ScTrigStat != 0) {
				Trigger();
				if (_ScTimCtr != 0) {
						_ScTimCtr--;
				}
				else {
					_ScTimCtr = _ScTimScl;
					Copy2Buf();				
				}
			}
			else {
				if (_ScPstTrCtr != 0) {
					if (_ScTimCtr != 0) {
						_ScTimCtr--;
					}
					else {
						_ScTimCtr = _ScTimScl;
						Copy2Buf();
						_ScPstTrCtr--;				
					}
				}
			}		
		}
	}
}


