;;=======================================================================
;; COPYRIGHT (c) NXP B.V. 2002-2014
;; All rights are reserved. Reproduction in whole or in part is
;; prohibited without the written consent of the copyright owner.
;;=======================================================================
;; add this assembly file in each compile project

.undef global text _main
.undef global text eta_csp_isr0	
.undef global text eta_csp_isr1
.undef global text eta_csp_isr2	

;; the interrupt vector table with 8 interrupts
;; Important: the first 4 lines always need to use a goto statement!

 .text global 0 _ivt
	goto _main_init         ;  0
	goto eta_csp_isr0       ;  1
	goto eta_csp_isr1       ;  2
	goto eta_csp_isr2       ;  3
	ireturn                 ;  4
	ireturn                 ;  5
	ireturn                 ;  6
	ireturn                 ;  7

.text global 0 _empty_isr
	ireturn

;; initialisation before entering the main function.
.text global 0 _main_init
      sr.ie = 1                  ; enable interrupts
      sr.r = 1                   ; enable rounding
      sr.b = 0                   ; disable unbiased rounding
      sr.s = 1                   ; enable saturation
      dcall _main                ; 1 ds
      sp = _sp_start_value_XMEM  ; init SP (adjusted to stack in cf16.bcf)
      sr.ie = 0                  ; return here dis int
      eopc                       ; sleep here until restart
      goto _main_init
