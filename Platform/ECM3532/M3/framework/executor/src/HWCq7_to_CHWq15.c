#include <stdint.h>

void HWCq7_to_CHWq15(int8_t *in, int8_t *out, uint16_t H, uint16_t W, uint16_t C)
{
#if 1
  uint16_t kh, kw, kc;
  uint32_t HW = H*W;
  uint32_t WC = W*C;
  uint32_t hin, win, cin, hout, wout, cout, hwin, hwout;
  for (kh = 0; kh < H; kh++){
    hin = kh * WC;
    hout = kh * W;
    for (kw = 0; kw < W; kw++){
      win = kw * C;
      //wout = kw;
      hwin = hin + win;
      hwout = hout + kw; //wout;
      for (kc = 0; kc < C; kc++){
	//cin = kc;
	cout = kc * HW;
        *(out + cout + hwout) = (int8_t)(*(in + kc + hwin));
      }
    }
  }
#endif
}

void CHWq15_to_HWCq7(int8_t *in, int8_t *out, uint16_t H, uint16_t W, uint16_t C)
{
#if 1
  uint16_t kh, kw, kc;
  uint32_t HW = H*W;
  uint32_t WC = W*C;
  uint32_t hin, win, cin, hout, wout, cout, hwin, hwout;
  for (kh = 0; kh < H; kh++){
    hout = kh * WC;
    hin = kh * W;
    for (kw = 0; kw < W; kw++){
      wout = kw * C;
      //win = kw;
      hwout = hout + wout;
      hwin = hin + kw; //win;
      for (kc = 0; kc < C; kc++){
	//cout = kc;
	cin = kc * HW;
        *(out + kc + hwout) = (int8_t)(*(in + cin + hwin));
      }
    }
  }
#endif
}
