#ifndef D3DHELP_H
#define D3DHELP_H


class CD3DFont;

const DWORD eiColorRed		= 0xffff0000;
const DWORD eiColorGreen	= 0xff00ff00;
const DWORD eiColorBlue		= 0xff0000ff;



const char* D3DHELP_GetFormatString(DWORD format);
void Draw2DText(CD3DFont*, float x, float y, DWORD clr, const char* str);
HRESULT GetColorFromString(const char* clr, DWORD& dwColor);


#endif