
#include <stdint.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
typedef ap_axiu<8,2,5,6> uint8_sc;
void imgStret(hls::stream<uint8_sc> &inStream,
		hls::stream<uint8_sc> &outStream,
		unsigned char xMin,unsigned char xMax)
{
#pragma HLS INTERFACE s_axilite port=return bundle=CRTL
#pragma HLS INTERFACE s_axilite port=xMax bundle=CRTL
#pragma HLS INTERFACE s_axilite port=xMin bundle=CRTL
#pragma HLS INTERFACE axis register both port=outStream
#pragma HLS INTERFACE axis register both port=inStream
	float xMax_xMin = xMax-xMin;
	for(int i=0;i<320*240;i++)
	{
#pragma HLS PIPELINE
		uint8_sc currPixelSc = inStream.read();
		uint8_sc outPixelSc;
		unsigned char x_t = currPixelSc.data;
		float y_t_float = ((x_t-xMin)/(xMax_xMin))*255;
		unsigned char y_t = y_t_float;
		outPixelSc.data = y_t;
		outPixelSc.keep a= currPixelSc.keep;
		outPixelSc.strb= currPixelSc.strb;
		outPixelSc.user= currPixelSc.user;
		outPixelSc.last=currPixelSc.last;
		outPixelSc.id=currPixelSc.id;
		outPixelSc.dest=currPixelSc.dest;

		outStream.write(outPixelSc);
	}



}
