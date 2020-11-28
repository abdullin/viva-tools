	.586p
	ifdef ??version
	if    ??version GT 500H
	.mmx
	endif
	endif
	model flat
	ifndef	??version
	?debug	macro
	endm
	endif
	?debug	S "F:\DAIODeliverable\VivaSW\Asm.cpp"
	?debug	T "F:\DAIODeliverable\VivaSW\Asm.cpp"
_TEXT	segment dword public use32 'CODE'
_TEXT	ends
_DATA	segment dword public use32 'DATA'
_DATA	ends
_BSS	segment dword public use32 'BSS'
_BSS	ends
$$BSYMS	segment byte public use32 'DEBSYM'
$$BSYMS	ends
$$BTYPES	segment byte public use32 'DEBTYP'
$$BTYPES	ends
$$BNAMES	segment byte public use32 'DEBNAM'
$$BNAMES	ends
$$BROWSE	segment byte public use32 'DEBSYM'
$$BROWSE	ends
$$BROWFILE	segment byte public use32 'DEBSYM'
$$BROWFILE	ends
 ?debug  C FB0C0100600000
DGROUP	group	_BSS,_DATA
_DATA	segment dword public use32 'DATA'
	align	4
_hDlPortIO	label	dword
	dd	0
	align	4
_lpDlReadPortUshort	label	dword
	dd	0
	align	4
_lpDlWritePortUchar	label	dword
	dd	0
	align	4
_lpDlWritePortUshort	label	dword
	dd	0
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@outportb$qqrusuc	segment virtual
@@outportb$qqrusuc	proc	near
?live16385@0:
	?debug L 30
	push      ebp
	mov       ebp,esp
	push      ecx
	mov       byte ptr [ebp-3],dl
	mov       word ptr [ebp-2],ax
	?debug L 32
@1:
	mov       eax,dword ptr [_lpDlWritePortUchar]
	test      eax,eax
	je        short @2
	?debug L 33
	mov       dl,byte ptr [ebp-3]
	push      edx
	movzx     ecx,word ptr [ebp-2]
	push      ecx
	call      dword ptr [_lpDlWritePortUchar]
	?debug L 34
@2:
@3:
	pop       ecx
	pop       ebp
	ret 
	?debug L 0
@@outportb$qqrusuc	endp
@outportb$qqrusuc	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	db	2
	db	0
	db	0
	db	0
	dw	64
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch1
	dd	?patch2
	dd	?patch3
	df	@@outportb$qqrusuc
	dw	0
	dw	4096
	dw	0
	dw	1
	dw	0
	dw	0
	dw	0
	db	17
	db	64
	db	111
	db	117
	db	116
	db	112
	db	111
	db	114
	db	116
	db	98
	db	36
	db	113
	db	113
	db	114
	db	117
	db	115
	db	117
	db	99
	dw	18
	dw	512
	dw	65534
	dw	65535
	dw	33
	dw	0
	dw	2
	dw	0
	dw	0
	dw	0
	dw	?patch4
	dw	529
	dw	?patch5
	dd	?live16385@0-@@outportb$qqrusuc
	dd	?live16385@0-?live16385@0
	dw	9
?patch5	equ	1
?patch4	equ	14
	dw	18
	dw	512
	dw	65533
	dw	65535
	dw	32
	dw	0
	dw	3
	dw	0
	dw	0
	dw	0
	dw	?patch6
	dw	529
	dw	?patch7
	dd	?live16385@0-@@outportb$qqrusuc
	dd	?live16385@0-?live16385@0
	dw	3
?patch7	equ	1
?patch6	equ	14
?patch1	equ	@3-@@outportb$qqrusuc+3
?patch2	equ	0
?patch3	equ	@3-@@outportb$qqrusuc
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@outport$qqrusus	segment virtual
@@outport$qqrusus	proc	near
?live16386@0:
	?debug L 37
	push      ebp
	mov       ebp,esp
	push      ecx
	mov       word ptr [ebp-4],dx
	mov       word ptr [ebp-2],ax
	?debug L 39
@4:
	mov       eax,dword ptr [_lpDlWritePortUshort]
	test      eax,eax
	je        short @5
	?debug L 40
	mov       dx,word ptr [ebp-4]
	push      edx
	movzx     ecx,word ptr [ebp-2]
	push      ecx
	call      dword ptr [_lpDlWritePortUshort]
	?debug L 41
@5:
@6:
	pop       ecx
	pop       ebp
	ret 
	?debug L 0
@@outport$qqrusus	endp
@outport$qqrusus	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	63
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch8
	dd	?patch9
	dd	?patch10
	df	@@outport$qqrusus
	dw	0
	dw	4098
	dw	0
	dw	4
	dw	0
	dw	0
	dw	0
	db	16
	db	64
	db	111
	db	117
	db	116
	db	112
	db	111
	db	114
	db	116
	db	36
	db	113
	db	113
	db	114
	db	117
	db	115
	db	117
	db	115
	dw	18
	dw	512
	dw	65534
	dw	65535
	dw	33
	dw	0
	dw	5
	dw	0
	dw	0
	dw	0
	dw	?patch11
	dw	529
	dw	?patch12
	dd	?live16386@0-@@outport$qqrusus
	dd	?live16386@0-?live16386@0
	dw	9
?patch12	equ	1
?patch11	equ	14
	dw	18
	dw	512
	dw	65532
	dw	65535
	dw	33
	dw	0
	dw	6
	dw	0
	dw	0
	dw	0
	dw	?patch13
	dw	529
	dw	?patch14
	dd	?live16386@0-@@outport$qqrusus
	dd	?live16386@0-?live16386@0
	dw	11
?patch14	equ	1
?patch13	equ	14
?patch8	equ	@6-@@outport$qqrusus+3
?patch9	equ	0
?patch10	equ	@6-@@outport$qqrusus
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@inport$qqrus	segment virtual
@@inport$qqrus	proc	near
?live16387@0:
	?debug L 44
	push      ebp
	mov       ebp,esp
	push      ecx
	mov       word ptr [ebp-2],ax
	?debug L 46
@7:
	mov       eax,dword ptr [_lpDlReadPortUshort]
	test      eax,eax
	je        short @8
	?debug L 47
	movzx     edx,word ptr [ebp-2]
	push      edx
	call      dword ptr [_lpDlReadPortUshort]
@12:
	pop       ecx
	pop       ebp
	ret 
	?debug L 49
@8:
	mov       ax,-1
	?debug L 50
@11:
@9:
	pop       ecx
	pop       ebp
	ret 
	?debug L 0
@@inport$qqrus	endp
@inport$qqrus	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	60
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch15
	dd	?patch16
	dd	?patch17
	df	@@inport$qqrus
	dw	0
	dw	4100
	dw	0
	dw	7
	dw	0
	dw	0
	dw	0
	db	13
	db	64
	db	105
	db	110
	db	112
	db	111
	db	114
	db	116
	db	36
	db	113
	db	113
	db	114
	db	117
	db	115
	dw	18
	dw	512
	dw	65534
	dw	65535
	dw	33
	dw	0
	dw	8
	dw	0
	dw	0
	dw	0
	dw	?patch18
	dw	529
	dw	?patch19
	dd	?live16387@0-@@inport$qqrus
	dd	?live16387@0-?live16387@0
	dw	9
?patch19	equ	1
?patch18	equ	14
	dw	8
	dw	530
	dd	@12-@@inport$qqrus
	dw	3
?patch15	equ	@11-@@inport$qqrus+3
?patch16	equ	0
?patch17	equ	@11-@@inport$qqrus
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_DATA	segment dword public use32 'DATA'
@_$ECTNF$@program@$bdtr$qv	segment virtual
@@_$ECTNF$@program@$bdtr$qv	label	dword
	dd	0
	dd	-36
@_$ECTNF$@program@$bdtr$qv	ends
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@program@$bdtr$qv	segment virtual
@@program@$bdtr$qv	proc	near
?live16388@0:
	?debug L 53
	push      ebp
	mov       ebp,esp
	add       esp,-40
@13:
	mov       eax,offset @@_$ECTNF$@program@$bdtr$qv
	call      @__InitExceptBlockLDTC
	mov       edx,dword ptr [ebp+8]
	test      edx,edx
	je        short @14
	?debug L 55
@15:
	xor       ecx,ecx
	mov       dword ptr [ebp-40],ecx
	?debug L 57
@16:
	mov       eax,dword ptr [ebp-40]
	mov       edx,dword ptr [ebp+8]
	mov       ecx,dword ptr [edx+4*eax]
	test      ecx,ecx
	je        short @18
	?debug L 59
	push      3
	mov       eax,dword ptr [ebp-40]
	mov       edx,dword ptr [ebp+8]
	push      dword ptr [edx+4*eax]
	call      @@ChipData@$bdtr$qv
	add       esp,8
	?debug L 60
	mov       ecx,dword ptr [ebp-40]
	mov       eax,dword ptr [ebp+8]
	xor       edx,edx
	mov       dword ptr [eax+4*ecx],edx
	?debug L 55
@18:
@19:
	inc       dword ptr [ebp-40]
	mov       ecx,dword ptr [ebp-40]
	cmp       ecx,10
	jl        short @16
	test      byte ptr [ebp+12],1
	je        short @21
	push      dword ptr [ebp+8]
	call      @$bdele$qpv
	pop       ecx
	?debug L 63
@21:
@14:
	mov       eax,dword ptr [ebp-36]
	mov       dword ptr fs:[0],eax
	?debug L 63
@22:
	mov       esp,ebp
	pop       ebp
	ret 
	?debug L 0
@@program@$bdtr$qv	endp
@program@$bdtr$qv	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	64
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch20
	dd	?patch21
	dd	?patch22
	df	@@program@$bdtr$qv
	dw	0
	dw	4154
	dw	0
	dw	41
	dw	0
	dw	0
	dw	0
	db	17
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	36
	db	98
	db	100
	db	116
	db	114
	db	36
	db	113
	db	118
	dw	18
	dw	512
	dw	8
	dw	0
	dw	4129
	dw	0
	dw	42
	dw	0
	dw	0
	dw	0
	dw	24
	dw	519
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch23
	df	@15
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65496
	dw	65535
	dw	116
	dw	0
	dw	43
	dw	0
	dw	0
	dw	0
?patch23	equ	@19-@15
	dw	2
	dw	6
?patch20	equ	@22-@@program@$bdtr$qv+4
?patch21	equ	0
?patch22	equ	@22-@@program@$bdtr$qv
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@program@SetPorts$qusus	segment virtual
@@program@SetPorts$qusus	proc	near
?live16390@0:
	?debug L 66
	push      ebp
	mov       ebp,esp
	?debug L 68
@23:
	mov       ax,word ptr [ebp+16]
	mov       edx,dword ptr [ebp+8]
	mov       word ptr [edx+42],ax
	?debug L 69
	mov       cx,word ptr [ebp+12]
	mov       eax,dword ptr [ebp+8]
	mov       word ptr [eax+44],cx
	?debug L 70
@24:
	pop       ebp
	ret 
	?debug L 0
@@program@SetPorts$qusus	endp
@program@SetPorts$qusus	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	70
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch24
	dd	?patch25
	dd	?patch26
	df	@@program@SetPorts$qusus
	dw	0
	dw	4190
	dw	0
	dw	44
	dw	0
	dw	0
	dw	0
	db	23
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	83
	db	101
	db	116
	db	80
	db	111
	db	114
	db	116
	db	115
	db	36
	db	113
	db	117
	db	115
	db	117
	db	115
	dw	18
	dw	512
	dw	8
	dw	0
	dw	4129
	dw	0
	dw	45
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	12
	dw	0
	dw	33
	dw	0
	dw	46
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	16
	dw	0
	dw	33
	dw	0
	dw	47
	dw	0
	dw	0
	dw	0
?patch24	equ	@24-@@program@SetPorts$qusus+2
?patch25	equ	0
?patch26	equ	@24-@@program@SetPorts$qusus
	dw	2
	dw	6
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@program@delay$qqrul	segment virtual
@@program@delay$qqrul	proc	near
?live16391@0:
	?debug L 73
	push      ebp
	mov       ebp,esp
	add       esp,-12
	mov       dword ptr [ebp-8],edx
	mov       dword ptr [ebp-4],eax
	?debug L 75
@25:
	mov       eax,dword ptr [ebp-8]
	mov       dword ptr [ebp-12],eax
	?debug L 77
	mov       ecx,30
	mov       eax,dword ptr [ebp-12]
	xor       edx,edx
	div       ecx
	mov       dword ptr [ebp-12],eax
	?debug L 78
	inc       dword ptr [ebp-12]
	?debug L 79
	push      dword ptr [ebp-12]
	call      @Sleep
	?debug L 80
	mov       eax,dword ptr [ebp-12]
	?debug L 81
@27:
@26:
	mov       esp,ebp
	pop       ebp
	ret 
	?debug L 0
@@program@delay$qqrul	endp
@program@delay$qqrul	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	67
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch27
	dd	?patch28
	dd	?patch29
	df	@@program@delay$qqrul
	dw	0
	dw	4192
	dw	0
	dw	48
	dw	0
	dw	0
	dw	0
	db	20
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	100
	db	101
	db	108
	db	97
	db	121
	db	36
	db	113
	db	113
	db	114
	db	117
	db	108
	dw	18
	dw	512
	dw	65532
	dw	65535
	dw	4129
	dw	0
	dw	49
	dw	0
	dw	0
	dw	0
	dw	?patch30
	dw	529
	dw	?patch31
	dd	?live16391@0-@@program@delay$qqrul
	dd	?live16391@0-?live16391@0
	dw	17
?patch31	equ	1
?patch30	equ	14
	dw	18
	dw	512
	dw	65528
	dw	65535
	dw	34
	dw	0
	dw	50
	dw	0
	dw	0
	dw	0
	dw	?patch32
	dw	529
	dw	?patch33
	dd	?live16391@0-@@program@delay$qqrul
	dd	?live16391@0-?live16391@0
	dw	19
?patch33	equ	1
?patch32	equ	14
	dw	18
	dw	512
	dw	65524
	dw	65535
	dw	34
	dw	0
	dw	51
	dw	0
	dw	0
	dw	0
?patch27	equ	@27-@@program@delay$qqrul+4
?patch28	equ	0
?patch29	equ	@27-@@program@delay$qqrul
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_DATA	segment dword public use32 'DATA'
@_$DCOF$@program@SetFormat$qipc	segment virtual
@@_$DCOF$@program@SetFormat$qipc	label	dword
	dd	@@$xt$p8ChipData
	dd	8199
	dd	-4
	dd	0
@_$DCOF$@program@SetFormat$qipc	ends
_DATA	ends
_DATA	segment dword public use32 'DATA'
@_$ECTPF$@program@SetFormat$qipc	segment virtual
@@_$ECTPF$@program@SetFormat$qipc	label	dword
	dd	0
	dd	-40
	dw	0
	dw	5
	dd	0
	dd	0
	dw	8
	dw	5
	dd	0
	dd	@@_$DCOF$@program@SetFormat$qipc
@_$ECTPF$@program@SetFormat$qipc	ends
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@program@SetFormat$qipc	segment virtual
@@program@SetFormat$qipc	proc	near
?live16392@0:
	?debug L 84
	push      ebp
	mov       ebp,esp
	add       esp,-44
@28:
	mov       eax,offset @@_$ECTPF$@program@SetFormat$qipc
	call      @__InitExceptBlockLDTC
	?debug L 86
	push      12
	call      @$bnew$qui
	pop       ecx
	mov       dword ptr [ebp-4],eax
	test      eax,eax
	je        short @29
	mov       word ptr [ebp-24],20
	push      dword ptr [ebp+16]
	push      dword ptr [ebp-4]
	call      @@ChipData@$bctr$qpc
	add       esp,8
	mov       word ptr [ebp-24],8
	mov       edx,dword ptr [ebp-4]
	jmp       short @30
@29:
	mov       edx,dword ptr [ebp-4]
@30:
	mov       dword ptr [ebp-44],edx
	?debug L 88
	push      dword ptr [ebp-44]
	call      @@ChipData@GetError$qv
	pop       ecx
	test      eax,eax
	jne       short @31
	?debug L 90
	mov       eax,dword ptr [ebp+12]
	mov       ecx,dword ptr [ebp+8]
	mov       edx,dword ptr [ebp-44]
	mov       dword ptr [ecx+4*eax],edx
	?debug L 92
	xor       eax,eax
	mov       ecx,dword ptr [ebp-40]
	mov       dword ptr fs:[0],ecx
	jmp       short @32
	?debug L 96
@31:
	push      3
	push      dword ptr [ebp-44]
	call      @@ChipData@$bdtr$qv
	add       esp,8
	?debug L 98
	mov       ax,1
	mov       edx,dword ptr [ebp-40]
	mov       dword ptr fs:[0],edx
	?debug L 100
@34:
@32:
	mov       esp,ebp
	pop       ebp
	ret 
	?debug L 0
@@program@SetFormat$qipc	endp
@program@SetFormat$qipc	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	70
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch34
	dd	?patch35
	dd	?patch36
	df	@@program@SetFormat$qipc
	dw	0
	dw	4194
	dw	0
	dw	52
	dw	0
	dw	0
	dw	0
	db	23
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	83
	db	101
	db	116
	db	70
	db	111
	db	114
	db	109
	db	97
	db	116
	db	36
	db	113
	db	105
	db	112
	db	99
	dw	18
	dw	512
	dw	8
	dw	0
	dw	4129
	dw	0
	dw	53
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	12
	dw	0
	dw	116
	dw	0
	dw	54
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	16
	dw	0
	dw	1136
	dw	0
	dw	55
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65492
	dw	65535
	dw	4105
	dw	0
	dw	56
	dw	0
	dw	0
	dw	0
?patch34	equ	@34-@@program@SetFormat$qipc+4
?patch35	equ	0
?patch36	equ	@34-@@program@SetFormat$qipc
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@program@InitializeProg$qv	segment virtual
@@program@InitializeProg$qv	proc	near
?live16397@0:
	?debug L 103
	push      ebp
	mov       ebp,esp
	?debug L 105
@35:
	mov       eax,dword ptr [ebp+8]
	mov       dx,word ptr [eax+44]
	mov       eax,dword ptr [ebp+8]
	call      @@program@X2InitChips$qqrus
	?debug L 106
@36:
	pop       ebp
	ret 
	?debug L 0
@@program@InitializeProg$qv	endp
@program@InitializeProg$qv	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	73
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch37
	dd	?patch38
	dd	?patch39
	df	@@program@InitializeProg$qv
	dw	0
	dw	4196
	dw	0
	dw	57
	dw	0
	dw	0
	dw	0
	db	26
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	73
	db	110
	db	105
	db	116
	db	105
	db	97
	db	108
	db	105
	db	122
	db	101
	db	80
	db	114
	db	111
	db	103
	db	36
	db	113
	db	118
	dw	18
	dw	512
	dw	8
	dw	0
	dw	4129
	dw	0
	dw	58
	dw	0
	dw	0
	dw	0
?patch37	equ	@36-@@program@InitializeProg$qv+2
?patch38	equ	0
?patch39	equ	@36-@@program@InitializeProg$qv
	dw	2
	dw	6
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@program@InitializeArray$qusus	segment virtual
@@program@InitializeArray$qusus	proc	near
?live16398@0:
	?debug L 109
	push      ebp
	mov       ebp,esp
	push      ecx
	?debug L 111
@37:
	mov       ax,word ptr [ebp+16]
	mov       word ptr [ebp-2],ax
	?debug L 112
	mov       edx,dword ptr [ebp+8]
	mov       cx,word ptr [edx+40]
	not       cx
	mov       word ptr [ebp-4],cx
	?debug L 113
	mov       dx,-1
	mov       ax,word ptr [ebp-2]
	call      @@outport$qqrusus
	?debug L 114
	mov       edx,100
	mov       eax,dword ptr [ebp+8]
	call      @@program@delay$qqrul
	?debug L 115
	mov       dx,word ptr [ebp-4]
	mov       ax,word ptr [ebp-2]
	call      @@outport$qqrusus
	?debug L 116
	mov       edx,100
	mov       eax,dword ptr [ebp+8]
	call      @@program@delay$qqrul
	?debug L 117
	mov       dx,-1
	mov       ax,word ptr [ebp-2]
	call      @@outport$qqrusus
	?debug L 118
	mov       edx,2000
	mov       eax,dword ptr [ebp+8]
	call      @@program@delay$qqrul
	?debug L 119
@38:
	pop       ecx
	pop       ebp
	ret 
	?debug L 0
@@program@InitializeArray$qusus	endp
@program@InitializeArray$qusus	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	77
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch40
	dd	?patch41
	dd	?patch42
	df	@@program@InitializeArray$qusus
	dw	0
	dw	4198
	dw	0
	dw	59
	dw	0
	dw	0
	dw	0
	db	30
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	73
	db	110
	db	105
	db	116
	db	105
	db	97
	db	108
	db	105
	db	122
	db	101
	db	65
	db	114
	db	114
	db	97
	db	121
	db	36
	db	113
	db	117
	db	115
	db	117
	db	115
	dw	18
	dw	512
	dw	8
	dw	0
	dw	4129
	dw	0
	dw	60
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	12
	dw	0
	dw	33
	dw	0
	dw	61
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	16
	dw	0
	dw	33
	dw	0
	dw	62
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65532
	dw	65535
	dw	33
	dw	0
	dw	63
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65534
	dw	65535
	dw	33
	dw	0
	dw	64
	dw	0
	dw	0
	dw	0
?patch40	equ	@38-@@program@InitializeArray$qusus+3
?patch41	equ	0
?patch42	equ	@38-@@program@InitializeArray$qusus
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_DATA	segment dword public use32 'DATA'
	align	4
_CurrentBytePointer	label	dword
	dd	0
	align	4
_CurrentBitPointer	label	dword
	dd	0
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@program@ProgramArray$qv	segment virtual
@@program@ProgramArray$qv	proc	near
?live16399@0:
	?debug L 125
	push      ebp
	mov       ebp,esp
	add       esp,-16
	?debug L 127
@39:
	xor       eax,eax
	mov       dword ptr [_CurrentBytePointer],eax
	?debug L 128
	xor       edx,edx
	mov       dword ptr [_CurrentBitPointer],edx
	?debug L 130
	push      dword ptr [ebp+8]
	call      @@program@CreateProgramMask$qv
	pop       ecx
	mov       dword ptr [ebp-4],eax
	?debug L 132
	mov       ecx,dword ptr [ebp-4]
	test      ecx,ecx
	je        @41
	?debug L 134
	mov       eax,dword ptr [ebp+8]
	mov       dx,word ptr [eax+42]
	push      edx
	mov       ecx,dword ptr [ebp+8]
	mov       ax,word ptr [ecx+40]
	push      eax
	push      dword ptr [ebp+8]
	call      @@program@InitializeArray$qusus
	add       esp,12
	?debug L 136
@42:
	mov       eax,dword ptr [ebp-4]
	mov       ecx,100
	cdq
	idiv      ecx
	inc       eax
	mov       dword ptr [ebp-8],eax
	mov       eax,dword ptr [ebp-8]
	mov       dword ptr [ebp-12],eax
	?debug L 138
	xor       edx,edx
	mov       dword ptr [ebp-16],edx
	?debug L 142
@43:
	mov       ecx,dword ptr [_CurrentBytePointer]
	mov       eax,dword ptr [ebp-12]
	cmp       ecx,eax
	jl        short @44
	?debug L 144
	mov       edx,dword ptr [ebp-8]
	add       dword ptr [ebp-12],edx
	?debug L 145
	push      -1
	inc       dword ptr [ebp-16]
	mov       ecx,dword ptr [ebp-16]
	push      ecx
	call      @@VivaGlobals@SetProgPos$qii
	add       esp,8
	?debug L 148
@44:
	xor       eax,eax
	mov       dword ptr [_CurrentBitPointer],eax
	jmp       short @46
	?debug L 149
@45:
	push      dword ptr [ebp+8]
	call      @@program@CreateProgramWord$qv
	pop       ecx
	mov       edx,eax
	mov       ecx,dword ptr [ebp+8]
	mov       ax,word ptr [ecx+42]
	add       ax,2
	call      @@outport$qqrusus
	inc       dword ptr [_CurrentBitPointer]
@46:
	mov       edx,dword ptr [_CurrentBitPointer]
	cmp       edx,8
	jl        short @45
	?debug L 151
	inc       dword ptr [_CurrentBytePointer]
	?debug L 153
	mov       ecx,dword ptr [ebp-4]
	mov       eax,dword ptr [_CurrentBytePointer]
	cmp       ecx,eax
	jg        short @43
	?debug L 156
@49:
@41:
	push      -1
	push      0
	call      @@VivaGlobals@SetProgPos$qii
	add       esp,8
	?debug L 157
	xor       eax,eax
	?debug L 158
@51:
@50:
	mov       esp,ebp
	pop       ebp
	ret 
	?debug L 0
@@program@ProgramArray$qv	endp
@program@ProgramArray$qv	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	71
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch43
	dd	?patch44
	dd	?patch45
	df	@@program@ProgramArray$qv
	dw	0
	dw	4200
	dw	0
	dw	65
	dw	0
	dw	0
	dw	0
	db	24
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	80
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	65
	db	114
	db	114
	db	97
	db	121
	db	36
	db	113
	db	118
	dw	18
	dw	512
	dw	8
	dw	0
	dw	4129
	dw	0
	dw	66
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65532
	dw	65535
	dw	18
	dw	0
	dw	67
	dw	0
	dw	0
	dw	0
	dw	24
	dw	519
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch46
	df	@42
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65520
	dw	65535
	dw	116
	dw	0
	dw	68
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65524
	dw	65535
	dw	116
	dw	0
	dw	69
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65528
	dw	65535
	dw	116
	dw	0
	dw	70
	dw	0
	dw	0
	dw	0
?patch46	equ	@49-@42
	dw	2
	dw	6
?patch43	equ	@51-@@program@ProgramArray$qv+4
?patch44	equ	0
?patch45	equ	@51-@@program@ProgramArray$qv
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@program@CreateProgramWord$qv	segment virtual
@@program@CreateProgramWord$qv	proc	near
?live16400@0:
	?debug L 160
	push      ebp
	mov       ebp,esp
	add       esp,-20
	?debug L 162
@52:
	mov       word ptr [ebp-2],0
	?debug L 164
@53:
	mov       dword ptr [ebp-8],1
	?debug L 166
@54:
@56:
	mov       eax,dword ptr [ebp+8]
	mov       edx,dword ptr [ebp-8]
	mov       ecx,dword ptr [eax+4*edx]
	mov       eax,dword ptr [ecx]
	mov       dword ptr [ebp-12],eax
	?debug L 168
	mov       edx,dword ptr [ebp-12]
	test      edx,edx
	je        short @57
	?debug L 171
@58:
	mov       ecx,dword ptr [_CurrentBytePointer]
	mov       eax,dword ptr [ebp-12]
	mov       dl,byte ptr [eax+ecx]
	mov       byte ptr [ebp-13],dl
	?debug L 172
	mov       ecx,7
	mov       eax,dword ptr [_CurrentBitPointer]
	sub       ecx,eax
	xor       edx,edx
	mov       dl,byte ptr [ebp-13]
	sar       edx,cl
	and       edx,1
	mov       dword ptr [ebp-20],edx
	?debug L 173
	mov       ecx,16
	mov       eax,dword ptr [ebp-8]
	sub       ecx,eax
	mov       dx,word ptr [ebp-20]
	shl       dx,cl
	or        word ptr [ebp-2],dx
	?debug L 164
@59:
@57:
@60:
@61:
	inc       dword ptr [ebp-8]
	mov       ecx,dword ptr [ebp-8]
	cmp       ecx,10
	jl        short @54
	?debug L 177
	mov       ax,word ptr [ebp-2]
	?debug L 178
@64:
@63:
	mov       esp,ebp
	pop       ebp
	ret 
	?debug L 0
@@program@CreateProgramWord$qv	endp
@program@CreateProgramWord$qv	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	76
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch47
	dd	?patch48
	dd	?patch49
	df	@@program@CreateProgramWord$qv
	dw	0
	dw	4202
	dw	0
	dw	71
	dw	0
	dw	0
	dw	0
	db	29
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	67
	db	114
	db	101
	db	97
	db	116
	db	101
	db	80
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	87
	db	111
	db	114
	db	100
	db	36
	db	113
	db	118
	dw	18
	dw	512
	dw	8
	dw	0
	dw	4129
	dw	0
	dw	72
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65534
	dw	65535
	dw	33
	dw	0
	dw	73
	dw	0
	dw	0
	dw	0
	dw	24
	dw	519
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch50
	df	@53
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65528
	dw	65535
	dw	116
	dw	0
	dw	74
	dw	0
	dw	0
	dw	0
	dw	24
	dw	519
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch51
	df	@56
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65524
	dw	65535
	dw	1056
	dw	0
	dw	75
	dw	0
	dw	0
	dw	0
	dw	24
	dw	519
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch52
	df	@58
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65516
	dw	65535
	dw	116
	dw	0
	dw	76
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65523
	dw	65535
	dw	32
	dw	0
	dw	77
	dw	0
	dw	0
	dw	0
?patch52	equ	@59-@58
	dw	2
	dw	6
?patch51	equ	@60-@56
	dw	2
	dw	6
?patch50	equ	@61-@53
	dw	2
	dw	6
?patch47	equ	@64-@@program@CreateProgramWord$qv+4
?patch48	equ	0
?patch49	equ	@64-@@program@CreateProgramWord$qv
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@program@CreateProgramMask$qv	segment virtual
@@program@CreateProgramMask$qv	proc	near
?live16401@0:
	?debug L 180
	push      ebp
	mov       ebp,esp
	add       esp,-8
	?debug L 182
@65:
	mov       eax,dword ptr [ebp+8]
	mov       word ptr [eax+40],0
	?debug L 184
	xor       edx,edx
	mov       dword ptr [ebp-4],edx
	?debug L 186
@66:
	mov       dword ptr [ebp-8],1
	?debug L 188
@67:
	mov       ecx,dword ptr [ebp-8]
	mov       eax,dword ptr [ebp+8]
	mov       edx,dword ptr [eax+4*ecx]
	test      edx,edx
	je        short @69
	?debug L 190
	mov       ecx,dword ptr [ebp-8]
	mov       eax,dword ptr [ebp+8]
	push      dword ptr [eax+4*ecx]
	call      @@ChipData@GetLength$qv
	pop       ecx
	mov       edx,dword ptr [ebp-4]
	cmp       eax,edx
	jle       short @70
	?debug L 191
	mov       ecx,dword ptr [ebp-8]
	mov       eax,dword ptr [ebp+8]
	push      dword ptr [eax+4*ecx]
	call      @@ChipData@GetLength$qv
	pop       ecx
	mov       dword ptr [ebp-4],eax
	?debug L 193
@70:
	mov       ecx,16
	mov       edx,dword ptr [ebp-8]
	sub       ecx,edx
	mov       ax,1
	shl       ax,cl
	mov       edx,dword ptr [ebp+8]
	or        word ptr [edx+40],ax
	?debug L 186
@69:
@71:
	inc       dword ptr [ebp-8]
	mov       ecx,dword ptr [ebp-8]
	cmp       ecx,10
	jl        short @67
	?debug L 197
	mov       eax,dword ptr [ebp-4]
	?debug L 198
@74:
@73:
	pop       ecx
	pop       ecx
	pop       ebp
	ret 
	?debug L 0
@@program@CreateProgramMask$qv	endp
@program@CreateProgramMask$qv	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	76
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch53
	dd	?patch54
	dd	?patch55
	df	@@program@CreateProgramMask$qv
	dw	0
	dw	4204
	dw	0
	dw	78
	dw	0
	dw	0
	dw	0
	db	29
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	67
	db	114
	db	101
	db	97
	db	116
	db	101
	db	80
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	77
	db	97
	db	115
	db	107
	db	36
	db	113
	db	118
	dw	18
	dw	512
	dw	8
	dw	0
	dw	4129
	dw	0
	dw	79
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65532
	dw	65535
	dw	18
	dw	0
	dw	80
	dw	0
	dw	0
	dw	0
	dw	24
	dw	519
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch56
	df	@66
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65528
	dw	65535
	dw	116
	dw	0
	dw	81
	dw	0
	dw	0
	dw	0
?patch56	equ	@71-@66
	dw	2
	dw	6
?patch53	equ	@74-@@program@CreateProgramMask$qv+4
?patch54	equ	0
?patch55	equ	@74-@@program@CreateProgramMask$qv
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@program@ConfigureProg$qi	segment virtual
@@program@ConfigureProg$qi	proc	near
?live16402@0:
	?debug L 200
	push      ebp
	mov       ebp,esp
	?debug L 202
@75:
	push      dword ptr [ebp+8]
	call      @@program@InitializeProg$qv
	pop       ecx
	?debug L 203
	mov       eax,dword ptr [ebp+8]
	push      dword ptr [eax]
	call      @@ChipData@GetLength$qv
	pop       ecx
	push      eax
	mov       edx,dword ptr [ebp+8]
	push      dword ptr [edx]
	call      @@ChipData@GetDataBuffer$qv
	pop       ecx
	mov       ecx,eax
	mov       eax,dword ptr [ebp+8]
	mov       dx,word ptr [eax+44]
	mov       eax,dword ptr [ebp+8]
	call      @@program@DoProgProg$qqruspcl
	?debug L 204
	xor       eax,eax
	?debug L 205
@77:
@76:
	pop       ebp
	ret 
	?debug L 0
@@program@ConfigureProg$qi	endp
@program@ConfigureProg$qi	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	72
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch57
	dd	?patch58
	dd	?patch59
	df	@@program@ConfigureProg$qi
	dw	0
	dw	4206
	dw	0
	dw	82
	dw	0
	dw	0
	dw	0
	db	25
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	67
	db	111
	db	110
	db	102
	db	105
	db	103
	db	117
	db	114
	db	101
	db	80
	db	114
	db	111
	db	103
	db	36
	db	113
	db	105
	dw	18
	dw	512
	dw	8
	dw	0
	dw	4129
	dw	0
	dw	83
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	12
	dw	0
	dw	116
	dw	0
	dw	84
	dw	0
	dw	0
	dw	0
?patch57	equ	@77-@@program@ConfigureProg$qi+2
?patch58	equ	0
?patch59	equ	@77-@@program@ConfigureProg$qi
	dw	2
	dw	6
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@program@DoConfigure$qus	segment virtual
@@program@DoConfigure$qus	proc	near
?live16403@0:
	?debug L 207
	push      ebp
	mov       ebp,esp
	?debug L 209
@78:
	cmp       word ptr [ebp+12],1
	jne       short @79
	?debug L 211
	mov       eax,dword ptr [ebp+8]
	mov       edx,dword ptr [eax]
	test      edx,edx
	je        short @80
	?debug L 212
	push      0
	push      dword ptr [ebp+8]
	call      @@program@ConfigureProg$qi
	add       esp,8
	?debug L 213
@80:
	push      dword ptr [ebp+8]
	call      @@program@ProgramArray$qv
	pop       ecx
	?debug L 214
	jmp       short @81
	?debug L 215
@79:
	cmp       word ptr [ebp+12],0
	jne       short @82
	?debug L 217
	mov       ecx,dword ptr [ebp+8]
	push      dword ptr [ecx]
	call      @@ChipData@GetLength$qv
	pop       ecx
	push      eax
	mov       eax,dword ptr [ebp+8]
	push      dword ptr [eax]
	call      @@ChipData@GetDataBuffer$qv
	pop       ecx
	mov       ecx,eax
	mov       edx,dword ptr [ebp+8]
	mov       dx,word ptr [edx+44]
	mov       eax,dword ptr [ebp+8]
	call      @@program@DoProgVideo$qqruspcl
	?debug L 218
	jmp       short @81
	?debug L 219
@82:
	cmp       word ptr [ebp+12],2
	jne       short @83
	?debug L 221
	push      dword ptr [ebp+8]
	call      @@program@InitializeProg$qv
	pop       ecx
	?debug L 222
	mov       ecx,dword ptr [ebp+8]
	push      dword ptr [ecx+4]
	call      @@ChipData@GetDataBuffer$qv
	pop       ecx
	push      eax
	mov       eax,dword ptr [ebp+8]
	push      dword ptr [eax]
	call      @@ChipData@GetLength$qv
	pop       ecx
	push      eax
	mov       edx,dword ptr [ebp+8]
	push      dword ptr [edx]
	call      @@ChipData@GetDataBuffer$qv
	pop       ecx
	mov       ecx,eax
	mov       eax,dword ptr [ebp+8]
	mov       dx,word ptr [eax+44]
	mov       eax,dword ptr [ebp+8]
	call      @@program@DoProgAce2$qqruspct2l
	?debug L 225
@83:
@81:
	xor       eax,eax
	?debug L 226
@85:
@84:
	pop       ebp
	ret 
	?debug L 0
@@program@DoConfigure$qus	endp
@program@DoConfigure$qus	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	71
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch60
	dd	?patch61
	dd	?patch62
	df	@@program@DoConfigure$qus
	dw	0
	dw	4208
	dw	0
	dw	85
	dw	0
	dw	0
	dw	0
	db	24
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	68
	db	111
	db	67
	db	111
	db	110
	db	102
	db	105
	db	103
	db	117
	db	114
	db	101
	db	36
	db	113
	db	117
	db	115
	dw	18
	dw	512
	dw	8
	dw	0
	dw	4129
	dw	0
	dw	86
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	12
	dw	0
	dw	33
	dw	0
	dw	87
	dw	0
	dw	0
	dw	0
?patch60	equ	@85-@@program@DoConfigure$qus+2
?patch61	equ	0
?patch62	equ	@85-@@program@DoConfigure$qus
	dw	2
	dw	6
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@program@DoProgVideo$qqruspcl	segment virtual
@@program@DoProgVideo$qqruspcl	proc	near
?live16404@0:
	?debug L 228
	push      ebp
	mov       ebp,esp
	add       esp,-28
	push      ebx
	mov       dword ptr [ebp-12],ecx
	mov       word ptr [ebp-6],dx
	mov       dword ptr [ebp-4],eax
	?debug L 236
@86:
		mov	 dx,word ptr [ebp-6]           
	?debug L 238
		mov		word ptr [ebp-22],dx
	?debug L 239
	xor       eax,eax
	mov       dword ptr [ebp-28],eax
	?debug L 241
	xor       edx,edx
	mov       dword ptr [ebp-20],edx
	mov       ecx,dword ptr [ebp-20]
	mov       eax,dword ptr [ebp+8]
	cmp       ecx,eax
	jge       @88
	?debug L 243
@87:
	mov       eax,dword ptr [ebp+8]
	mov       ecx,100
	cdq
	idiv      ecx
	push      eax
	mov       eax,dword ptr [ebp-20]
	pop       edx
	mov       ecx,edx
	cdq
	idiv      ecx
	mov       edx,dword ptr [ebp-28]
	cmp       eax,edx
	jle       short @89
	?debug L 245
	mov       eax,dword ptr [ebp+8]
	mov       ecx,100
	cdq
	idiv      ecx
	push      eax
	mov       eax,dword ptr [ebp-20]
	pop       edx
	mov       ecx,edx
	cdq
	idiv      ecx
	mov       dword ptr [ebp-28],eax
	?debug L 246
	push      -1
	push      dword ptr [ebp-28]
	call      @@VivaGlobals@SetProgPos$qii
	add       esp,8
	?debug L 248
@89:
	mov       eax,dword ptr [ebp-12]
	mov       edx,dword ptr [ebp-20]
	mov       al,byte ptr [eax+edx]
	mov       byte ptr [ebp-13],al
	?debug L 249
 	mov	 dx,word ptr [ebp-22]
	?debug L 250
 	mov	 bl,byte ptr [ebp-13]     
	?debug L 251
 	mov	 bh,0
	?debug L 252
 	mov	 ecx,8     
	?debug L 254
@90:
 	shl	 bx,1       
	?debug L 255
 	mov	 al,bh
	?debug L 256
 	and	 al,1       
	?debug L 257
 	shl	 ax,1
	?debug L 258
 	out	 dx,al      
	?debug L 259
	loop      @90
@91:
	inc       dword ptr [ebp-20]
	mov       ecx,dword ptr [ebp-20]
	mov       eax,dword ptr [ebp+8]
	cmp       ecx,eax
	jl        @87
	?debug L 263
@88:
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 264
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 265
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 266
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 267
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 269
@93:
@92:
	pop       ebx
	mov       esp,ebp
	pop       ebp
	ret       4
	?debug L 0
@@program@DoProgVideo$qqruspcl	endp
@program@DoProgVideo$qqruspcl	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	76
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch63
	dd	?patch64
	dd	?patch65
	df	@@program@DoProgVideo$qqruspcl
	dw	0
	dw	4210
	dw	0
	dw	88
	dw	0
	dw	0
	dw	0
	db	29
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	68
	db	111
	db	80
	db	114
	db	111
	db	103
	db	86
	db	105
	db	100
	db	101
	db	111
	db	36
	db	113
	db	113
	db	114
	db	117
	db	115
	db	112
	db	99
	db	108
	dw	18
	dw	512
	dw	65532
	dw	65535
	dw	4129
	dw	0
	dw	89
	dw	0
	dw	0
	dw	0
	dw	?patch66
	dw	529
	dw	?patch67
	dd	?live16404@0-@@program@DoProgVideo$qqruspcl
	dd	?live16404@0-?live16404@0
	dw	17
?patch67	equ	1
?patch66	equ	14
	dw	18
	dw	512
	dw	65530
	dw	65535
	dw	33
	dw	0
	dw	90
	dw	0
	dw	0
	dw	0
	dw	?patch68
	dw	529
	dw	?patch69
	dd	?live16404@0-@@program@DoProgVideo$qqruspcl
	dd	?live16404@0-?live16404@0
	dw	11
?patch69	equ	1
?patch68	equ	14
	dw	18
	dw	512
	dw	65524
	dw	65535
	dw	1136
	dw	0
	dw	91
	dw	0
	dw	0
	dw	0
	dw	?patch70
	dw	529
	dw	?patch71
	dd	?live16404@0-@@program@DoProgVideo$qqruspcl
	dd	?live16404@0-?live16404@0
	dw	18
?patch71	equ	1
?patch70	equ	14
	dw	18
	dw	512
	dw	8
	dw	0
	dw	18
	dw	0
	dw	92
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65508
	dw	65535
	dw	18
	dw	0
	dw	93
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65514
	dw	65535
	dw	33
	dw	0
	dw	94
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65516
	dw	65535
	dw	18
	dw	0
	dw	95
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65523
	dw	65535
	dw	32
	dw	0
	dw	96
	dw	0
	dw	0
	dw	0
?patch63	equ	@93-@@program@DoProgVideo$qqruspcl+7
?patch64	equ	0
?patch65	equ	@93-@@program@DoProgVideo$qqruspcl
	dw	2
	dw	6
	dw	8
	dw	531
	dw	1
	dw	65504
	dw	65535
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@program@DoProgAce2$qqruspct2l	segment virtual
@@program@DoProgAce2$qqruspct2l	proc	near
?live16405@0:
	?debug L 271
	push      ebp
	mov       ebp,esp
	add       esp,-28
	push      ebx
	mov       dword ptr [ebp-12],ecx
	mov       word ptr [ebp-6],dx
	mov       dword ptr [ebp-4],eax
	?debug L 277
@94:
	mov       dx,word ptr [ebp-6]
	mov       eax,dword ptr [ebp-4]
	call      @@program@X2InitChips$qqrus
	?debug L 279
		mov	 dx,word ptr [ebp-6]           
	?debug L 280
		add	 dx,2          
	?debug L 281
		mov		word ptr [ebp-22],dx
	?debug L 283
	xor       ecx,ecx
	mov       dword ptr [ebp-28],ecx
	?debug L 285
	xor       eax,eax
	mov       dword ptr [ebp-20],eax
	mov       edx,dword ptr [ebp-20]
	mov       ecx,dword ptr [ebp+8]
	cmp       edx,ecx
	jge       @96
	?debug L 287
@95:
	mov       eax,dword ptr [ebp+8]
	mov       ecx,100
	cdq
	idiv      ecx
	push      eax
	mov       eax,dword ptr [ebp-20]
	pop       edx
	mov       ecx,edx
	cdq
	idiv      ecx
	mov       edx,dword ptr [ebp-28]
	cmp       eax,edx
	jle       short @97
	?debug L 289
	mov       eax,dword ptr [ebp+8]
	mov       ecx,100
	cdq
	idiv      ecx
	push      eax
	mov       eax,dword ptr [ebp-20]
	pop       edx
	mov       ecx,edx
	cdq
	idiv      ecx
	mov       dword ptr [ebp-28],eax
	?debug L 290
	push      -1
	push      dword ptr [ebp-28]
	call      @@VivaGlobals@SetProgPos$qii
	add       esp,8
	?debug L 293
@97:
	mov       eax,dword ptr [ebp-12]
	mov       edx,dword ptr [ebp-20]
	mov       al,byte ptr [eax+edx]
	mov       byte ptr [ebp-13],al
	?debug L 294
	mov       ecx,dword ptr [ebp+12]
	mov       eax,dword ptr [ebp-20]
	mov       dl,byte ptr [ecx+eax]
	mov       byte ptr [ebp-14],dl
	?debug L 295
 	mov	 bl,byte ptr [ebp-13]     
	?debug L 296
 	mov	 bh,0
	?debug L 297
 	mov	 dl,byte ptr [ebp-14]
	?debug L 298
 	mov	 dh,0
	?debug L 299
 	mov	 ecx,8     
	?debug L 301
@98:
 	shl	 bx,1       
	?debug L 302
 	shl	 dx,1
	?debug L 303
 	mov	 al,bh
	?debug L 304
 	and	 al,1       
	?debug L 305
 	shl	 al,7
	?debug L 306
 	and	 dh,1
	?debug L 307
 	or	 al,dh
	?debug L 308
 	push	 dx
	?debug L 309
 	mov	 dx,word ptr [ebp-22]
	?debug L 310
 	out	 dx,al      
	?debug L 311
 	pop	 dx
	?debug L 312
	loop      @98
@99:
	inc       dword ptr [ebp-20]
	mov       ecx,dword ptr [ebp-20]
	mov       eax,dword ptr [ebp+8]
	cmp       ecx,eax
	jl        @95
	?debug L 316
@96:
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 317
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 318
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 319
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 320
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 322
@101:
@100:
	pop       ebx
	mov       esp,ebp
	pop       ebp
	ret       8
	?debug L 0
@@program@DoProgAce2$qqruspct2l	endp
@program@DoProgAce2$qqruspct2l	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	77
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch72
	dd	?patch73
	dd	?patch74
	df	@@program@DoProgAce2$qqruspct2l
	dw	0
	dw	4212
	dw	0
	dw	97
	dw	0
	dw	0
	dw	0
	db	30
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	68
	db	111
	db	80
	db	114
	db	111
	db	103
	db	65
	db	99
	db	101
	db	50
	db	36
	db	113
	db	113
	db	114
	db	117
	db	115
	db	112
	db	99
	db	116
	db	50
	db	108
	dw	18
	dw	512
	dw	65532
	dw	65535
	dw	4129
	dw	0
	dw	98
	dw	0
	dw	0
	dw	0
	dw	?patch75
	dw	529
	dw	?patch76
	dd	?live16405@0-@@program@DoProgAce2$qqruspct2l
	dd	?live16405@0-?live16405@0
	dw	17
?patch76	equ	1
?patch75	equ	14
	dw	18
	dw	512
	dw	65530
	dw	65535
	dw	33
	dw	0
	dw	99
	dw	0
	dw	0
	dw	0
	dw	?patch77
	dw	529
	dw	?patch78
	dd	?live16405@0-@@program@DoProgAce2$qqruspct2l
	dd	?live16405@0-?live16405@0
	dw	11
?patch78	equ	1
?patch77	equ	14
	dw	18
	dw	512
	dw	65524
	dw	65535
	dw	1136
	dw	0
	dw	100
	dw	0
	dw	0
	dw	0
	dw	?patch79
	dw	529
	dw	?patch80
	dd	?live16405@0-@@program@DoProgAce2$qqruspct2l
	dd	?live16405@0-?live16405@0
	dw	18
?patch80	equ	1
?patch79	equ	14
	dw	18
	dw	512
	dw	12
	dw	0
	dw	1136
	dw	0
	dw	101
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	8
	dw	0
	dw	18
	dw	0
	dw	102
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65508
	dw	65535
	dw	18
	dw	0
	dw	103
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65514
	dw	65535
	dw	33
	dw	0
	dw	104
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65516
	dw	65535
	dw	18
	dw	0
	dw	105
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65522
	dw	65535
	dw	32
	dw	0
	dw	106
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65523
	dw	65535
	dw	32
	dw	0
	dw	107
	dw	0
	dw	0
	dw	0
?patch72	equ	@101-@@program@DoProgAce2$qqruspct2l+7
?patch73	equ	0
?patch74	equ	@101-@@program@DoProgAce2$qqruspct2l
	dw	2
	dw	6
	dw	8
	dw	531
	dw	1
	dw	65504
	dw	65535
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@program@DoProgProg$qqruspcl	segment virtual
@@program@DoProgProg$qqruspcl	proc	near
?live16406@0:
	?debug L 325
	push      ebp
	mov       ebp,esp
	add       esp,-36
	mov       dword ptr [ebp-12],ecx
	mov       word ptr [ebp-6],dx
	mov       dword ptr [ebp-4],eax
	?debug L 331
@102:
	mov       dx,word ptr [ebp-6]
	mov       eax,dword ptr [ebp-4]
	call      @@program@X2InitChips$qqrus
	?debug L 333
	mov       cx,word ptr [ebp-6]
	add       cx,2
	mov       word ptr [ebp-22],cx
	?debug L 337
	mov       eax,dword ptr [ebp+8]
	mov       ecx,100
	cdq
	idiv      ecx
	inc       eax
	mov       dword ptr [ebp-28],eax
	mov       eax,dword ptr [ebp-28]
	mov       dword ptr [ebp-32],eax
	?debug L 338
	xor       edx,edx
	mov       dword ptr [ebp-36],edx
	?debug L 340
	xor       ecx,ecx
	mov       dword ptr [ebp-20],ecx
	mov       eax,dword ptr [ebp-20]
	mov       edx,dword ptr [ebp+8]
	cmp       eax,edx
	jge       @104
	?debug L 342
@103:
	mov       ecx,dword ptr [ebp-20]
	mov       eax,dword ptr [ebp-32]
	cmp       ecx,eax
	jl        short @105
	?debug L 344
	mov       edx,dword ptr [ebp-28]
	add       dword ptr [ebp-32],edx
	?debug L 345
	push      -1
	inc       dword ptr [ebp-36]
	mov       ecx,dword ptr [ebp-36]
	push      ecx
	call      @@VivaGlobals@SetProgPos$qii
	add       esp,8
	?debug L 348
@105:
	mov       eax,dword ptr [ebp-12]
	mov       edx,dword ptr [ebp-20]
	mov       cl,byte ptr [eax+edx]
	mov       byte ptr [ebp-13],cl
	?debug L 351
	mov	 dx,word ptr [ebp-22]
	?debug L 352
	mov	 ah,byte ptr [ebp-13]
	?debug L 353
	mov	 ecx,8     
	?debug L 354
@106:
nxt7:
	?debug L 355
	rol	 ax,1
	?debug L 356
	and	 al,1       
	?debug L 357
	out	 dx,al      
	?debug L 358
	dec	 ecx
	?debug L 359
	jne       @106
@107:
	inc       dword ptr [ebp-20]
	mov       eax,dword ptr [ebp-20]
	mov       edx,dword ptr [ebp+8]
	cmp       eax,edx
	jl        @103
	?debug L 365
@104:
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 366
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 367
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 368
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 369
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 371
@109:
@108:
	mov       esp,ebp
	pop       ebp
	ret       4
	?debug L 0
@@program@DoProgProg$qqruspcl	endp
@program@DoProgProg$qqruspcl	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	75
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch81
	dd	?patch82
	dd	?patch83
	df	@@program@DoProgProg$qqruspcl
	dw	0
	dw	4214
	dw	0
	dw	108
	dw	0
	dw	0
	dw	0
	db	28
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	68
	db	111
	db	80
	db	114
	db	111
	db	103
	db	80
	db	114
	db	111
	db	103
	db	36
	db	113
	db	113
	db	114
	db	117
	db	115
	db	112
	db	99
	db	108
	dw	18
	dw	512
	dw	65532
	dw	65535
	dw	4129
	dw	0
	dw	109
	dw	0
	dw	0
	dw	0
	dw	?patch84
	dw	529
	dw	?patch85
	dd	?live16406@0-@@program@DoProgProg$qqruspcl
	dd	?live16406@0-?live16406@0
	dw	17
?patch85	equ	1
?patch84	equ	14
	dw	18
	dw	512
	dw	65530
	dw	65535
	dw	33
	dw	0
	dw	110
	dw	0
	dw	0
	dw	0
	dw	?patch86
	dw	529
	dw	?patch87
	dd	?live16406@0-@@program@DoProgProg$qqruspcl
	dd	?live16406@0-?live16406@0
	dw	11
?patch87	equ	1
?patch86	equ	14
	dw	18
	dw	512
	dw	65524
	dw	65535
	dw	1136
	dw	0
	dw	111
	dw	0
	dw	0
	dw	0
	dw	?patch88
	dw	529
	dw	?patch89
	dd	?live16406@0-@@program@DoProgProg$qqruspcl
	dd	?live16406@0-?live16406@0
	dw	18
?patch89	equ	1
?patch88	equ	14
	dw	18
	dw	512
	dw	8
	dw	0
	dw	18
	dw	0
	dw	112
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65500
	dw	65535
	dw	116
	dw	0
	dw	113
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65504
	dw	65535
	dw	116
	dw	0
	dw	114
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65508
	dw	65535
	dw	116
	dw	0
	dw	115
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65514
	dw	65535
	dw	33
	dw	0
	dw	116
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65516
	dw	65535
	dw	18
	dw	0
	dw	117
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65523
	dw	65535
	dw	32
	dw	0
	dw	118
	dw	0
	dw	0
	dw	0
?patch81	equ	@109-@@program@DoProgProg$qqruspcl+6
?patch82	equ	0
?patch83	equ	@109-@@program@DoProgProg$qqruspcl
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@program@X2InitChips$qqrus	segment virtual
@@program@X2InitChips$qqrus	proc	near
?live16407@0:
	?debug L 373
	push      ebp
	mov       ebp,esp
	add       esp,-12
	mov       word ptr [ebp-6],dx
	mov       dword ptr [ebp-4],eax
	?debug L 375
@110:
	movzx     eax,word ptr [ebp-6]
	mov       dword ptr [ebp-12],eax
	?debug L 376
	mov       dl,7
	mov       ax,word ptr [ebp-12]
	call      @@outportb$qqrusuc
	?debug L 377
	mov       edx,10
	mov       eax,dword ptr [ebp-4]
	call      @@program@delay$qqrul
	?debug L 378
	mov       dl,6
	mov       ax,word ptr [ebp-12]
	call      @@outportb$qqrusuc
	?debug L 379
	mov       edx,10
	mov       eax,dword ptr [ebp-4]
	call      @@program@delay$qqrul
	?debug L 380
	xor       edx,edx
	mov       ax,word ptr [ebp-12]
	call      @@outportb$qqrusuc
	?debug L 381
	mov       edx,10
	mov       eax,dword ptr [ebp-4]
	call      @@program@delay$qqrul
	?debug L 382
	mov       dl,7
	mov       ax,word ptr [ebp-12]
	call      @@outportb$qqrusuc
	?debug L 383
	mov       edx,10
	mov       eax,dword ptr [ebp-4]
	call      @@program@delay$qqrul
	?debug L 384
	mov       dl,6
	mov       ax,word ptr [ebp-12]
	call      @@outportb$qqrusuc
	?debug L 385
	mov       edx,10
	mov       eax,dword ptr [ebp-4]
	call      @@program@delay$qqrul
	?debug L 386
	mov       dl,7
	mov       ax,word ptr [ebp-12]
	call      @@outportb$qqrusuc
	?debug L 387
	mov       edx,100
	mov       eax,dword ptr [ebp-4]
	call      @@program@delay$qqrul
	?debug L 388
@111:
	mov       esp,ebp
	pop       ebp
	ret 
	?debug L 0
@@program@X2InitChips$qqrus	endp
@program@X2InitChips$qqrus	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	73
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch90
	dd	?patch91
	dd	?patch92
	df	@@program@X2InitChips$qqrus
	dw	0
	dw	4216
	dw	0
	dw	119
	dw	0
	dw	0
	dw	0
	db	26
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	88
	db	50
	db	73
	db	110
	db	105
	db	116
	db	67
	db	104
	db	105
	db	112
	db	115
	db	36
	db	113
	db	113
	db	114
	db	117
	db	115
	dw	18
	dw	512
	dw	65532
	dw	65535
	dw	4129
	dw	0
	dw	120
	dw	0
	dw	0
	dw	0
	dw	?patch93
	dw	529
	dw	?patch94
	dd	?live16407@0-@@program@X2InitChips$qqrus
	dd	?live16407@0-?live16407@0
	dw	17
?patch94	equ	1
?patch93	equ	14
	dw	18
	dw	512
	dw	65530
	dw	65535
	dw	33
	dw	0
	dw	121
	dw	0
	dw	0
	dw	0
	dw	?patch95
	dw	529
	dw	?patch96
	dd	?live16407@0-@@program@X2InitChips$qqrus
	dd	?live16407@0-?live16407@0
	dw	11
?patch96	equ	1
?patch95	equ	14
	dw	18
	dw	512
	dw	65524
	dw	65535
	dw	116
	dw	0
	dw	122
	dw	0
	dw	0
	dw	0
?patch90	equ	@111-@@program@X2InitChips$qqrus+4
?patch91	equ	0
?patch92	equ	@111-@@program@X2InitChips$qqrus
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@program@ProgX2$qqruspcp21Comctrls@TProgressBarl	segment virtual
@@program@ProgX2$qqruspcp21Comctrls@TProgressBarl	proc	near
?live16408@0:
	?debug L 391
	push      ebp
	mov       ebp,esp
	add       esp,-28
	push      ebx
	mov       dword ptr [ebp-12],ecx
	mov       word ptr [ebp-6],dx
	mov       dword ptr [ebp-4],eax
	?debug L 397
@112:
	mov       dx,word ptr [ebp-6]
	mov       eax,dword ptr [ebp-4]
	call      @@program@X2InitChips$qqrus
	?debug L 399
		mov	 dx,word ptr [ebp-6]           
	?debug L 400
		add	 dx,2          
	?debug L 401
		mov		word ptr [ebp-22],dx
	?debug L 403
	xor       ecx,ecx
	mov       dword ptr [ebp-28],ecx
	?debug L 405
	xor       eax,eax
	mov       dword ptr [ebp-20],eax
	mov       edx,dword ptr [ebp-20]
	mov       ecx,dword ptr [ebp+8]
	cmp       edx,ecx
	jge       @114
	?debug L 407
@113:
	mov       eax,dword ptr [ebp+8]
	mov       ecx,100
	cdq
	idiv      ecx
	push      eax
	mov       eax,dword ptr [ebp-20]
	pop       edx
	mov       ecx,edx
	cdq
	idiv      ecx
	mov       edx,dword ptr [ebp-28]
	cmp       eax,edx
	jle       short @115
	?debug L 409
	mov       eax,dword ptr [ebp+8]
	mov       ecx,100
	cdq
	idiv      ecx
	push      eax
	mov       eax,dword ptr [ebp-20]
	pop       edx
	mov       ecx,edx
	cdq
	idiv      ecx
	mov       dword ptr [ebp-28],eax
	?debug L 410
	mov       edx,dword ptr [ebp-28]
	mov       eax,dword ptr [ebp+12]
	call      @@Comctrls@TProgressBar@SetPosition$qqri
	?debug L 412
@115:
	mov       ecx,dword ptr [ebp-12]
	mov       eax,dword ptr [ebp-20]
	mov       dl,byte ptr [ecx+eax]
	mov       byte ptr [ebp-13],dl
	?debug L 413
 	mov	 dx,word ptr [ebp-22]
	?debug L 414
 	mov	 bl,byte ptr [ebp-13]     
	?debug L 415
 	mov	 bh,0
	?debug L 416
 	mov	 ecx,8     
	?debug L 418
@116:
 	shl	 bx,1       
	?debug L 419
 	mov	 al,bh
	?debug L 420
 	and	 al,1       
	?debug L 421
 	out	 dx,al      
	?debug L 422
	loop      @116
@117:
	inc       dword ptr [ebp-20]
	mov       ecx,dword ptr [ebp-20]
	mov       eax,dword ptr [ebp+8]
	cmp       ecx,eax
	jl        @113
	?debug L 426
@114:
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 427
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 428
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 429
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 430
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 432
@119:
@118:
	pop       ebx
	mov       esp,ebp
	pop       ebp
	ret       8
	?debug L 0
@@program@ProgX2$qqruspcp21Comctrls@TProgressBarl	endp
@program@ProgX2$qqruspcp21Comctrls@TProgressBarl	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	95
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch97
	dd	?patch98
	dd	?patch99
	df	@@program@ProgX2$qqruspcp21Comctrls@TProgressBarl
	dw	0
	dw	4218
	dw	0
	dw	123
	dw	0
	dw	0
	dw	0
	db	48
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	80
	db	114
	db	111
	db	103
	db	88
	db	50
	db	36
	db	113
	db	113
	db	114
	db	117
	db	115
	db	112
	db	99
	db	112
	db	50
	db	49
	db	67
	db	111
	db	109
	db	99
	db	116
	db	114
	db	108
	db	115
	db	64
	db	84
	db	80
	db	114
	db	111
	db	103
	db	114
	db	101
	db	115
	db	115
	db	66
	db	97
	db	114
	db	108
	dw	18
	dw	512
	dw	65532
	dw	65535
	dw	4129
	dw	0
	dw	124
	dw	0
	dw	0
	dw	0
	dw	?patch100
	dw	529
	dw	?patch101
	dd	?live16408@0-@@program@ProgX2$qqruspcp21Comctrls@TProgressBarl
	dd	?live16408@0-?live16408@0
	dw	17
?patch101	equ	1
?patch100	equ	14
	dw	18
	dw	512
	dw	65530
	dw	65535
	dw	33
	dw	0
	dw	125
	dw	0
	dw	0
	dw	0
	dw	?patch102
	dw	529
	dw	?patch103
	dd	?live16408@0-@@program@ProgX2$qqruspcp21Comctrls@TProgressBarl
	dd	?live16408@0-?live16408@0
	dw	11
?patch103	equ	1
?patch102	equ	14
	dw	18
	dw	512
	dw	65524
	dw	65535
	dw	1136
	dw	0
	dw	126
	dw	0
	dw	0
	dw	0
	dw	?patch104
	dw	529
	dw	?patch105
	dd	?live16408@0-@@program@ProgX2$qqruspcp21Comctrls@TProgressBarl
	dd	?live16408@0-?live16408@0
	dw	18
?patch105	equ	1
?patch104	equ	14
	dw	18
	dw	512
	dw	12
	dw	0
	dw	65535
	dw	65535
	dw	127
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	8
	dw	0
	dw	18
	dw	0
	dw	128
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65508
	dw	65535
	dw	18
	dw	0
	dw	129
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65514
	dw	65535
	dw	33
	dw	0
	dw	130
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65516
	dw	65535
	dw	18
	dw	0
	dw	131
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65523
	dw	65535
	dw	32
	dw	0
	dw	132
	dw	0
	dw	0
	dw	0
?patch97	equ	@119-@@program@ProgX2$qqruspcp21Comctrls@TProgressBarl+7
?patch98	equ	0
?patch99	equ	@119-@@program@ProgX2$qqruspcp21Comctrls@TProgressBarl
	dw	2
	dw	6
	dw	8
	dw	531
	dw	1
	dw	65504
	dw	65535
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@program@ReProgX2$qqruspcp21Comctrls@TProgressBarl	segment virtual
@@program@ReProgX2$qqruspcp21Comctrls@TProgressBarl	proc	near
?live16409@0:
	?debug L 435
	push      ebp
	mov       ebp,esp
	add       esp,-24
	push      ebx
	mov       dword ptr [ebp-12],ecx
	mov       word ptr [ebp-6],dx
	mov       dword ptr [ebp-4],eax
	?debug L 441
@120:
		mov	     dx,word ptr [ebp-6]           
	?debug L 442
  	mov		al,0
	?debug L 443
  	out		dx,al                   
	?debug L 444
  	mov		cx,800         
	?debug L 446
@121:
  	out	 	dx,al
	?debug L 447
  	dec		cx
	?debug L 448
	jne       short @121
	?debug L 449
  	mov	 	al,0ffh
	?debug L 450
  	out	 	dx,al			
	?debug L 452
		add	     dx,2       
	?debug L 453
  	mov	     word ptr [ebp-22],dx
	?debug L 455
	xor       eax,eax
	mov       dword ptr [ebp-20],eax
	?debug L 457
@122:
	mov       edx,dword ptr [ebp-20]
	mov       ecx,dword ptr [ebp-12]
	mov       al,byte ptr [ecx+edx]
	mov       byte ptr [ebp-13],al
	?debug L 458
 	mov	 dx,word ptr [ebp-22]
	?debug L 459
 	mov	 bl,byte ptr [ebp-13]     
	?debug L 460
 	mov	 bh,0
	?debug L 461
 	mov	 cx,8     
	?debug L 463
@124:
 	shl	 bx,1       
	?debug L 464
 	mov	 al,bh
	?debug L 465
 	and	 al,1       
	?debug L 466
 	out	 dx,al      
	?debug L 467
	loop      @124
@125:
	inc       dword ptr [ebp-20]
	mov       edx,dword ptr [ebp-20]
	cmp       edx,179301
	jl        @122
	?debug L 470
@123:
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 471
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 472
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 473
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 474
	mov       dl,-1
	mov       ax,word ptr [ebp-22]
	call      @@outportb$qqrusuc
	?debug L 477
@127:
@126:
	pop       ebx
	mov       esp,ebp
	pop       ebp
	ret       8
	?debug L 0
@@program@ReProgX2$qqruspcp21Comctrls@TProgressBarl	endp
@program@ReProgX2$qqruspcp21Comctrls@TProgressBarl	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	97
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch106
	dd	?patch107
	dd	?patch108
	df	@@program@ReProgX2$qqruspcp21Comctrls@TProgressBarl
	dw	0
	dw	4220
	dw	0
	dw	133
	dw	0
	dw	0
	dw	0
	db	50
	db	64
	db	112
	db	114
	db	111
	db	103
	db	114
	db	97
	db	109
	db	64
	db	82
	db	101
	db	80
	db	114
	db	111
	db	103
	db	88
	db	50
	db	36
	db	113
	db	113
	db	114
	db	117
	db	115
	db	112
	db	99
	db	112
	db	50
	db	49
	db	67
	db	111
	db	109
	db	99
	db	116
	db	114
	db	108
	db	115
	db	64
	db	84
	db	80
	db	114
	db	111
	db	103
	db	114
	db	101
	db	115
	db	115
	db	66
	db	97
	db	114
	db	108
	dw	18
	dw	512
	dw	65532
	dw	65535
	dw	4129
	dw	0
	dw	134
	dw	0
	dw	0
	dw	0
	dw	?patch109
	dw	529
	dw	?patch110
	dd	?live16409@0-@@program@ReProgX2$qqruspcp21Comctrls@TProgressBarl
	dd	?live16409@0-?live16409@0
	dw	17
?patch110	equ	1
?patch109	equ	14
	dw	18
	dw	512
	dw	65530
	dw	65535
	dw	33
	dw	0
	dw	135
	dw	0
	dw	0
	dw	0
	dw	?patch111
	dw	529
	dw	?patch112
	dd	?live16409@0-@@program@ReProgX2$qqruspcp21Comctrls@TProgressBarl
	dd	?live16409@0-?live16409@0
	dw	11
?patch112	equ	1
?patch111	equ	14
	dw	18
	dw	512
	dw	65524
	dw	65535
	dw	1136
	dw	0
	dw	136
	dw	0
	dw	0
	dw	0
	dw	?patch113
	dw	529
	dw	?patch114
	dd	?live16409@0-@@program@ReProgX2$qqruspcp21Comctrls@TProgressBarl
	dd	?live16409@0-?live16409@0
	dw	18
?patch114	equ	1
?patch113	equ	14
	dw	18
	dw	512
	dw	12
	dw	0
	dw	65535
	dw	65535
	dw	137
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	8
	dw	0
	dw	18
	dw	0
	dw	138
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65514
	dw	65535
	dw	33
	dw	0
	dw	139
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65516
	dw	65535
	dw	18
	dw	0
	dw	140
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65523
	dw	65535
	dw	32
	dw	0
	dw	141
	dw	0
	dw	0
	dw	0
?patch106	equ	@127-@@program@ReProgX2$qqruspcp21Comctrls@TProgressBarl+7
?patch107	equ	0
?patch108	equ	@127-@@program@ReProgX2$qqruspcp21Comctrls@TProgressBarl
	dw	2
	dw	6
	dw	8
	dw	531
	dw	1
	dw	65508
	dw	65535
$$BSYMS	ends
_DATA	segment dword public use32 'DATA'
	align	4
_dseed	label	dword
	dd	40
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@IRand$qv	segment virtual
@@IRand$qv	proc	near
?live16410@0:
	?debug L 556
	push      ebp
	mov       ebp,esp
	?debug L 560
@128:
	push	    edx
	?debug L 561
	mov	     edx,33614
	?debug L 562
	mov	     eax,_dseed
	?debug L 563
	mul	     edx
	?debug L 564
	add	     edx,edx
	?debug L 565
	add	     eax,edx
	?debug L 566
	sbb	     edx,edx
	?debug L 567
	shr	     eax,1
	?debug L 568
	sub	     eax,edx
	?debug L 569
	mov	     _dseed,eax
	?debug L 570
	pop	     edx
	?debug L 573
	mov       eax,dword ptr [_dseed]
	?debug L 574
@130:
@129:
	pop       ebp
	ret 
	?debug L 0
@@IRand$qv	endp
@IRand$qv	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	56
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch115
	dd	?patch116
	dd	?patch117
	df	@@IRand$qv
	dw	0
	dw	4222
	dw	0
	dw	142
	dw	0
	dw	0
	dw	0
	db	9
	db	64
	db	73
	db	82
	db	97
	db	110
	db	100
	db	36
	db	113
	db	118
?patch115	equ	@130-@@IRand$qv+2
?patch116	equ	0
?patch117	equ	@130-@@IRand$qv
	dw	2
	dw	6
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@IRand$qi	segment virtual
@@IRand$qi	proc	near
?live16411@0:
	?debug L 578
	push      ebp
	mov       ebp,esp
	push      ecx
	?debug L 580
@131:
	call      @@IRand$qv
	mov       dword ptr [ebp-4],eax
	?debug L 584
	mov	     eax,dword ptr [ebp-4]
	?debug L 585
	add	     eax,eax
	?debug L 586
	mov	     edx,dword ptr [ebp+8]
	?debug L 587
	mul	     edx
	?debug L 588
	mov	     dword ptr [ebp-4],edx
	?debug L 591
	mov       eax,dword ptr [ebp-4]
	?debug L 592
@133:
@132:
	pop       ecx
	pop       ebp
	ret 
	?debug L 0
@@IRand$qi	endp
@IRand$qi	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	56
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch118
	dd	?patch119
	dd	?patch120
	df	@@IRand$qi
	dw	0
	dw	4224
	dw	0
	dw	143
	dw	0
	dw	0
	dw	0
	db	9
	db	64
	db	73
	db	82
	db	97
	db	110
	db	100
	db	36
	db	113
	db	105
	dw	18
	dw	512
	dw	8
	dw	0
	dw	116
	dw	0
	dw	144
	dw	0
	dw	0
	dw	0
	dw	18
	dw	512
	dw	65532
	dw	65535
	dw	116
	dw	0
	dw	145
	dw	0
	dw	0
	dw	0
?patch118	equ	@133-@@IRand$qi+3
?patch119	equ	0
?patch120	equ	@133-@@IRand$qi
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@RandomStart$qv	segment virtual
@@RandomStart$qv	proc	near
?live16412@0:
	?debug L 595
	push      ebp
	mov       ebp,esp
	?debug L 600
@134:
@135:
	pop       ebp
	ret 
	?debug L 0
@@RandomStart$qv	endp
@RandomStart$qv	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	62
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch121
	dd	?patch122
	dd	?patch123
	df	@@RandomStart$qv
	dw	0
	dw	4226
	dw	0
	dw	146
	dw	0
	dw	0
	dw	0
	db	15
	db	64
	db	82
	db	97
	db	110
	db	100
	db	111
	db	109
	db	83
	db	116
	db	97
	db	114
	db	116
	db	36
	db	113
	db	118
?patch121	equ	@135-@@RandomStart$qv+2
?patch122	equ	0
?patch123	equ	@135-@@RandomStart$qv
	dw	2
	dw	6
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@ChipData@GetError$qv	segment virtual
@@ChipData@GetError$qv	proc	near
?live16393@0:
	?debug	T "F:\DAIODeliverable\VivaSW\ChipData.h"
	?debug L 35
	push      ebp
	mov       ebp,esp
	?debug L 37
@136:
	mov       eax,dword ptr [ebp+8]
	mov       eax,dword ptr [eax+8]
	?debug L 38
@138:
@137:
	pop       ebp
	ret 
	?debug L 0
@@ChipData@GetError$qv	endp
@ChipData@GetError$qv	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	68
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch124
	dd	?patch125
	dd	?patch126
	df	@@ChipData@GetError$qv
	dw	0
	dw	4228
	dw	0
	dw	147
	dw	0
	dw	0
	dw	0
	db	21
	db	64
	db	67
	db	104
	db	105
	db	112
	db	68
	db	97
	db	116
	db	97
	db	64
	db	71
	db	101
	db	116
	db	69
	db	114
	db	114
	db	111
	db	114
	db	36
	db	113
	db	118
	dw	18
	dw	512
	dw	8
	dw	0
	dw	4108
	dw	0
	dw	148
	dw	0
	dw	0
	dw	0
?patch124	equ	@138-@@ChipData@GetError$qv+2
?patch125	equ	0
?patch126	equ	@138-@@ChipData@GetError$qv
	dw	2
	dw	6
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@$xt$p8ChipData	segment virtual
@@$xt$p8ChipData	label	dword
	dd	4
	dw	16
	dw	12
	dd	@@$xt$8ChipData
	db	67
	db	104
	db	105
	db	112
	db	68
	db	97
	db	116
	db	97
	db	32
	db	42
	db	0
@$xt$p8ChipData	ends
_TEXT	ends
_DATA	segment dword public use32 'DATA'
@TVivaTimer@	segment virtual
	align	2
@@TVivaTimer@	label	byte
	dd	@@TVivaTimer@+76
	dd	0
	dd	0
	dd	0
	dd	@@$xp$10TVivaTimer+4
	dd	0
	dd	0
	dd	0
	dd	@@TVivaTimer@+124
	dd	64
	dd	@Extctrls@TTimer@
	dd	@@Classes@TComponent@SafeCallException$qqrp14System@TObjectpv
	dd	@@System@TObject@AfterConstruction$qqrv
	dd	@@Classes@TComponent@BeforeDestruction$qqrv
	dd	@@System@TObject@Dispatch$qqrpv
	dd	@@System@TObject@DefaultHandler$qqrpv
	dd	@@System@TObject@NewInstance$qqrp17System@TMetaClass
	dd	@@System@TObject@FreeInstance$qqrv
	dd	@@TVivaTimer@$bdtr$qqrv
	dd	@@Classes@TPersistent@AssignTo$qqrp19Classes@TPersistent
	dd	@@Classes@TComponent@DefineProperties$qqrp14Classes@TFiler
	dd	@@Classes@TPersistent@Assign$qqrp19Classes@TPersistent
	dd	@@Classes@TComponent@Loaded$qqrv
	dd	@@Classes@TComponent@Notification$qqrp18Classes@TComponent18Classes@TOperation
	dd	@@Classes@TComponent@ReadState$qqrp15Classes@TReader
	dd	@@Classes@TComponent@SetName$qqrx17System@AnsiString
	dd	@@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2
	dd	@@Classes@TComponent@ValidateRename$qqrp18Classes@TComponentx17System@AnsiStringt2
	dd	@@Classes@TComponent@WriteState$qqrp15Classes@TWriter
	dd	@@Classes@TComponent@QueryInterface$qqsrx5_GUIDpv
	dd	@@TVivaTimer@$bctr$qqrp18Classes@TComponent
	db	10
	db        84,86,105,118,97,84,105,109,101,114
@TVivaTimer@	ends
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@$xp$10TVivaTimer	segment virtual
	align	2
@@$xp$10TVivaTimer	label	byte
	dd	@@$xp$10TVivaTimer+4
	db	7
	db	10
	db        84,86,105,118,97,84,105,109,101,114
	dd	@@TVivaTimer@+76
	dd	@$xp$15Extctrls@TTimer
	dw	5
	db	3
	db        65,115,109
	dw	0
@$xp$10TVivaTimer	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$8ChipData	segment virtual
@@$xt$8ChipData	label	byte
	dd	12
	dw	3
	dw	48
	dd	-1
	dd	3
	dw	60
	dw	64
	dd	0
	dw	0
	dw	0
	dd	0
	dd	1
	dd	1
	dd	@@ChipData@$bdtr$qv
	dw	1
	dw	68
	db	67
	db	104
	db	105
	db	112
	db	68
	db	97
	db	116
	db	97
	db	0
	db	0
	db	0
	db	0
	dd	0
	dd	0
	dd	0
@$xt$8ChipData	ends
_TEXT	ends
_DATA	segment dword public use32 'DATA'
@_$ECTRF$@TVivaTimer@$bdtr$qqrv	segment virtual
@@_$ECTRF$@TVivaTimer@$bdtr$qqrv	label	dword
	dd	0
	dd	-36
@_$ECTRF$@TVivaTimer@$bdtr$qqrv	ends
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@TVivaTimer@$bdtr$qqrv	segment virtual
@@TVivaTimer@$bdtr$qqrv	proc	near
?live16416@0:
	?debug L 0
	push      ebp
	mov       ebp,esp
	add       esp,-44
	call      __BeforeDestruction
	mov       byte ptr [ebp-41],dl
	mov       dword ptr [ebp-40],eax
@139:
	mov       eax,offset @@_$ECTRF$@TVivaTimer@$bdtr$qqrv
	call      @__InitExceptBlockLDTC
	mov       dl,byte ptr [ebp-41]
	test      dl,dl
	jl        short @140
	sub       dword ptr [ebp-8],5
	xor       edx,edx
	mov       eax,dword ptr [ebp-40]
	call      @@Extctrls@TTimer@$bdtr$qqrv
@140:
@141:
	mov       ecx,dword ptr [ebp-36]
	mov       dword ptr fs:[0],ecx
@143:
@142:
	mov       al,byte ptr [ebp-41]
	test      al,al
	jle       short @144
	mov       eax,dword ptr [ebp-40]
	call      __ClassDestroy
	?debug L 38
@144:
	mov       esp,ebp
	pop       ebp
	ret 
	?debug L 0
@@TVivaTimer@$bdtr$qqrv	endp
@TVivaTimer@$bdtr$qqrv	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	69
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch127
	dd	?patch128
	dd	?patch129
	df	@@TVivaTimer@$bdtr$qqrv
	dw	0
	dw	4247
	dw	0
	dw	154
	dw	0
	dw	0
	dw	0
	db	22
	db	64
	db	84
	db	86
	db	105
	db	118
	db	97
	db	84
	db	105
	db	109
	db	101
	db	114
	db	64
	db	36
	db	98
	db	100
	db	116
	db	114
	db	36
	db	113
	db	113
	db	114
	db	118
	dw	18
	dw	512
	dw	65496
	dw	65535
	dw	4236
	dw	0
	dw	155
	dw	0
	dw	0
	dw	0
	dw	?patch130
	dw	529
	dw	?patch131
	dd	?live16416@0-@@TVivaTimer@$bdtr$qqrv
	dd	?live16416@0-?live16416@0
	dw	17
?patch131	equ	1
?patch130	equ	14
?patch127	equ	@143-@@TVivaTimer@$bdtr$qqrv+19
?patch128	equ	0
?patch129	equ	@143-@@TVivaTimer@$bdtr$qqrv
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_DATA	segment dword public use32 'DATA'
@_$DCSF$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	segment virtual
@@_$DCSF$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	label	dword
	dd	@@$xt$17System@AnsiString
	dd	4
	dd	-4
	dd	@@$xt$17System@AnsiString
	dd	4
	dd	8
	dd	0
	dd	@@$xt$17System@AnsiString
	dd	4
	dd	-8
	dd	@@$xt$17System@AnsiString
	dd	4
	dd	-12
	dd	0
@_$DCSF$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	ends
_DATA	ends
_DATA	segment dword public use32 'DATA'
@_$ECTTF$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	segment virtual
@@_$ECTTF$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	label	dword
	dd	0
	dd	-48
	dw	0
	dw	5
	dd	2
	dd	@@_$DCSF$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2
	dw	8
	dw	5
	dd	4
	dd	@@_$DCSF$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2+28
@_$ECTTF$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	ends
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	segment virtual
@@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	proc	near
?live16417@0:
	?debug	T "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\classes.hpp"
	?debug L 2068
	push      ebp
	mov       ebp,esp
	add       esp,-56
	mov       dword ptr [ebp-4],ecx
	mov       byte ptr [ebp-53],dl
	mov       dword ptr [ebp-52],eax
@145:
	mov       eax,offset @@_$ECTTF$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2
	call      @__InitExceptBlockLDTC
	mov       dword ptr [ebp-20],2
	lea       edx,dword ptr [ebp-4]
	lea       eax,dword ptr [ebp-4]
	call      @@System@AnsiString@$bctr$qqrrx17System@AnsiString
	inc       dword ptr [ebp-20]
	?debug L 2069
	mov       word ptr [ebp-32],8
	lea       edx,dword ptr [ebp+8]
	lea       eax,dword ptr [ebp+8]
	call      @@System@AnsiString@$bctr$qqrrx17System@AnsiString
	inc       dword ptr [ebp-20]
	lea       edx,dword ptr [ebp+8]
	lea       eax,dword ptr [ebp-12]
	call      @@System@AnsiString@$bctr$qqrrx17System@AnsiString
	inc       dword ptr [ebp-20]
	push      dword ptr [eax]
	mov       word ptr [ebp-32],20
	lea       edx,dword ptr [ebp-4]
	lea       eax,dword ptr [ebp-8]
	call      @@System@AnsiString@$bctr$qqrrx17System@AnsiString
	inc       dword ptr [ebp-20]
	mov       ecx,dword ptr [eax]
	mov       dl,byte ptr [ebp-53]
	mov       eax,dword ptr [@Classes@TComponent@]
	call      @@Classes@TComponent@UpdateRegistry$qqrp17System@TMetaClassox17System@AnsiStringt3
	dec       dword ptr [ebp-20]
	lea       eax,dword ptr [ebp-12]
	mov       edx,2
	call      @@System@AnsiString@$bdtr$qqrv
	dec       dword ptr [ebp-20]
	lea       eax,dword ptr [ebp-8]
	mov       edx,2
	call      @@System@AnsiString@$bdtr$qqrv
	dec       dword ptr [ebp-20]
	lea       eax,dword ptr [ebp+8]
	mov       edx,2
	call      @@System@AnsiString@$bdtr$qqrv
	dec       dword ptr [ebp-20]
	lea       eax,dword ptr [ebp-4]
	mov       edx,2
	call      @@System@AnsiString@$bdtr$qqrv
	mov       ecx,dword ptr [ebp-48]
	mov       dword ptr fs:[0],ecx
	?debug L 2069
@146:
	mov       esp,ebp
	pop       ebp
	ret       4
	?debug L 0
@@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	endp
@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	108
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch132
	dd	?patch133
	dd	?patch134
	df	@@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2
	dw	0
	dw	4255
	dw	0
	dw	156
	dw	0
	dw	0
	dw	0
	db	61
	db	64
	db	67
	db	108
	db	97
	db	115
	db	115
	db	101
	db	115
	db	64
	db	84
	db	67
	db	111
	db	109
	db	112
	db	111
	db	110
	db	101
	db	110
	db	116
	db	64
	db	85
	db	112
	db	100
	db	97
	db	116
	db	101
	db	82
	db	101
	db	103
	db	105
	db	115
	db	116
	db	114
	db	121
	db	36
	db	113
	db	113
	db	114
	db	111
	db	120
	db	49
	db	55
	db	83
	db	121
	db	115
	db	116
	db	101
	db	109
	db	64
	db	65
	db	110
	db	115
	db	105
	db	83
	db	116
	db	114
	db	105
	db	110
	db	103
	db	116
	db	50
	dw	18
	dw	512
	dw	65484
	dw	65535
	dw	4256
	dw	0
	dw	157
	dw	0
	dw	0
	dw	0
	dw	?patch135
	dw	529
	dw	?patch136
	dd	?live16417@0-@@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2
	dd	?live16417@0-?live16417@0
	dw	17
?patch136	equ	1
?patch135	equ	14
	dw	18
	dw	512
	dw	65483
	dw	65535
	dw	48
	dw	0
	dw	158
	dw	0
	dw	0
	dw	0
	dw	?patch137
	dw	529
	dw	?patch138
	dd	?live16417@0-@@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2
	dd	?live16417@0-?live16417@0
	dw	3
?patch138	equ	1
?patch137	equ	14
	dw	18
	dw	512
	dw	65532
	dw	65535
	dw	4257
	dw	0
	dw	159
	dw	0
	dw	0
	dw	0
	dw	?patch139
	dw	529
	dw	?patch140
	dd	?live16417@0-@@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2
	dd	?live16417@0-?live16417@0
	dw	0
?patch140	equ	1
?patch139	equ	14
	dw	18
	dw	512
	dw	8
	dw	0
	dw	4257
	dw	0
	dw	160
	dw	0
	dw	0
	dw	0
?patch132	equ	@146-@@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2+6
?patch133	equ	0
?patch134	equ	@146-@@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_DATA	segment dword public use32 'DATA'
@_$DCUF$@TVivaTimer@$bctr$qqrp18Classes@TComponent	segment virtual
@@_$DCUF$@TVivaTimer@$bctr$qqrp18Classes@TComponent	label	dword
	dd	@@$xt$p10TVivaTimer
	dd	1029
	dd	-4
	dd	0
@_$DCUF$@TVivaTimer@$bctr$qqrp18Classes@TComponent	ends
_DATA	ends
_DATA	segment dword public use32 'DATA'
@_$ECTVF$@TVivaTimer@$bctr$qqrp18Classes@TComponent	segment virtual
@@_$ECTVF$@TVivaTimer@$bctr$qqrp18Classes@TComponent	label	dword
	dd	0
	dd	-40
	dw	0
	dw	5
	dd	0
	dd	@@_$DCUF$@TVivaTimer@$bctr$qqrp18Classes@TComponent
@_$ECTVF$@TVivaTimer@$bctr$qqrp18Classes@TComponent	ends
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@TVivaTimer@$bctr$qqrp18Classes@TComponent	segment virtual
@@TVivaTimer@$bctr$qqrp18Classes@TComponent	proc	near
?live16418@0:
	?debug	T "F:\DAIODeliverable\VivaSW\Globals.h"
	?debug L 1084
	push      ebp
	mov       ebp,esp
	add       esp,-56
	mov       byte ptr [ebp-8],dl
	test      dl,dl
	jle       short @147
	call      __ClassCreate
@147:
	mov       dword ptr [ebp-48],ecx
	mov       byte ptr [ebp-41],dl
	mov       dword ptr [ebp-4],eax
@148:
	mov       eax,offset @@_$ECTVF$@TVivaTimer@$bctr$qqrp18Classes@TComponent
	call      @__InitExceptBlockLDTC
	mov       word ptr [ebp-24],8
	mov       ecx,dword ptr [ebp-48]
	xor       edx,edx
	mov       eax,dword ptr [ebp-4]
	call      @@Extctrls@TTimer@$bctr$qqrp18Classes@TComponent
	add       dword ptr [ebp-12],5
	mov       dword ptr [ebp-56],offset @@TVivaTimer@TimerTimer$qqrp14System@TObject
	mov       edx,dword ptr [ebp-4]
	mov       dword ptr [ebp-52],edx
	lea       ecx,dword ptr [ebp-56]
	push      dword ptr [ecx+4]
	push      dword ptr [ecx]
	mov       eax,dword ptr [ebp-4]
	call      @@Extctrls@TTimer@SetOnTimer$qqrynpqqrp14System@TObject$v
	mov       edx,dword ptr [ebp-40]
	mov       dword ptr fs:[0],edx
	mov       eax,dword ptr [ebp-4]
@150:
@149:
	mov       dl,byte ptr [ebp-41]
	test      dl,dl
	je        short @151
	call      __AfterConstruction
	?debug L 1084
@151:
	mov       esp,ebp
	pop       ebp
	ret 
	?debug L 0
@@TVivaTimer@$bctr$qqrp18Classes@TComponent	endp
@TVivaTimer@$bctr$qqrp18Classes@TComponent	ends
_TEXT	ends
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	89
	dw	517
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dw	0
	dd	?patch141
	dd	?patch142
	dd	?patch143
	df	@@TVivaTimer@$bctr$qqrp18Classes@TComponent
	dw	0
	dw	4259
	dw	0
	dw	161
	dw	0
	dw	0
	dw	0
	db	42
	db	64
	db	84
	db	86
	db	105
	db	118
	db	97
	db	84
	db	105
	db	109
	db	101
	db	114
	db	64
	db	36
	db	98
	db	99
	db	116
	db	114
	db	36
	db	113
	db	113
	db	114
	db	112
	db	49
	db	56
	db	67
	db	108
	db	97
	db	115
	db	115
	db	101
	db	115
	db	64
	db	84
	db	67
	db	111
	db	109
	db	112
	db	111
	db	110
	db	101
	db	110
	db	116
	dw	18
	dw	512
	dw	65532
	dw	65535
	dw	4236
	dw	0
	dw	162
	dw	0
	dw	0
	dw	0
	dw	?patch144
	dw	529
	dw	?patch145
	dd	?live16418@0-@@TVivaTimer@$bctr$qqrp18Classes@TComponent
	dd	?live16418@0-?live16418@0
	dw	17
?patch145	equ	1
?patch144	equ	14
	dw	18
	dw	512
	dw	65488
	dw	65535
	dw	65533
	dw	65535
	dw	163
	dw	0
	dw	0
	dw	0
	dw	?patch146
	dw	529
	dw	?patch147
	dd	?live16418@0-@@TVivaTimer@$bctr$qqrp18Classes@TComponent
	dd	?live16418@0-?live16418@0
	dw	18
?patch147	equ	1
?patch146	equ	14
?patch141	equ	@150-@@TVivaTimer@$bctr$qqrp18Classes@TComponent+16
?patch142	equ	0
?patch143	equ	@150-@@TVivaTimer@$bctr$qqrp18Classes@TComponent
	dw	2
	dw	6
	dw	4
	dw	531
	dw	0
$$BSYMS	ends
_TEXT	segment dword public use32 'CODE'
@$xt$p10TVivaTimer	segment virtual
@@$xt$p10TVivaTimer	label	dword
	dd	4
	dw	144
	dw	12
	dd	@@$xt$10TVivaTimer
	db	84
	db	86
	db	105
	db	118
	db	97
	db	84
	db	105
	db	109
	db	101
	db	114
	db	32
	db	42
	db	0
@$xt$p10TVivaTimer	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$17System@AnsiString	segment virtual
@@$xt$17System@AnsiString	label	byte
	dd	4
	dw	3
	dw	48
	dd	-1
	dd	3
	dw	68
	dw	72
	dd	0
	dw	0
	dw	0
	dd	0
	dd	1
	dd	1
	dd	@@System@AnsiString@$bdtr$qqrv
	dw	3
	dw	76
	db	83
	db	121
	db	115
	db	116
	db	101
	db	109
	db	58
	db	58
	db	65
	db	110
	db	115
	db	105
	db	83
	db	116
	db	114
	db	105
	db	110
	db	103
	db	0
	db	0
	dd	0
	dd	0
	dd	0
@$xt$17System@AnsiString	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$10TVivaTimer	segment virtual
@@$xt$10TVivaTimer	label	byte
	dd	64
	dw	3
	dw	48
	dd	0
	dd	183
	dw	60
	dw	76
	dd	0
	dw	0
	dw	0
	dd	0
	dd	6
	dd	6
	dd	@@TVivaTimer@$bdtr$qqrv
	dw	3
	dw	80
	db	84
	db	86
	db	105
	db	118
	db	97
	db	84
	db	105
	db	109
	db	101
	db	114
	db	0
	db	0
	dd	@@$xt$15Extctrls@TTimer
	dd	0
	dd	3
	dd	0
	dd	0
	dd	0
@$xt$10TVivaTimer	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$15Extctrls@TTimer	segment virtual
@@$xt$15Extctrls@TTimer	label	byte
	dd	64
	dw	3
	dw	48
	dd	0
	dd	183
	dw	68
	dw	84
	dd	0
	dw	0
	dw	0
	dd	0
	dd	5
	dd	5
	dd	@@Extctrls@TTimer@$bdtr$qqrv
	dw	3
	dw	88
	db	69
	db	120
	db	116
	db	99
	db	116
	db	114
	db	108
	db	115
	db	58
	db	58
	db	84
	db	84
	db	105
	db	109
	db	101
	db	114
	db	0
	db	0
	db	0
	db	0
	dd	@@$xt$18Classes@TComponent
	dd	0
	dd	3
	dd	0
	dd	0
	dd	0
@$xt$15Extctrls@TTimer	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$18Classes@TComponent	segment virtual
@@$xt$18Classes@TComponent	label	byte
	dd	36
	dw	3
	dw	48
	dd	0
	dd	183
	dw	68
	dw	84
	dd	0
	dw	0
	dw	0
	dd	0
	dd	4
	dd	4
	dd	@@Classes@TComponent@$bdtr$qqrv
	dw	3
	dw	88
	db	67
	db	108
	db	97
	db	115
	db	115
	db	101
	db	115
	db	58
	db	58
	db	84
	db	67
	db	111
	db	109
	db	112
	db	111
	db	110
	db	101
	db	110
	db	116
	db	0
	dd	@@$xt$19Classes@TPersistent
	dd	0
	dd	3
	dd	0
	dd	0
	dd	@@$xt$17System@AnsiString
	dd	8
	dd	0
@$xt$18Classes@TComponent	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$19Classes@TPersistent	segment virtual
@@$xt$19Classes@TPersistent	label	byte
	dd	4
	dw	3
	dw	48
	dd	0
	dd	183
	dw	72
	dw	88
	dd	0
	dw	0
	dw	0
	dd	0
	dd	2
	dd	2
	dd	@@Classes@TPersistent@$bdtr$qqrv
	dw	3
	dw	92
	db	67
	db	108
	db	97
	db	115
	db	115
	db	101
	db	115
	db	58
	db	58
	db	84
	db	80
	db	101
	db	114
	db	115
	db	105
	db	115
	db	116
	db	101
	db	110
	db	116
	db	0
	db	0
	db	0
	db	0
	dd	@@$xt$14System@TObject
	dd	0
	dd	3
	dd	0
	dd	0
	dd	0
@$xt$19Classes@TPersistent	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$14System@TObject	segment virtual
@@$xt$14System@TObject	label	byte
	dd	4
	dw	3
	dw	48
	dd	0
	dd	179
	dw	64
	dw	68
	dd	0
	dw	0
	dw	0
	dd	0
	dd	1
	dd	1
	dd	@@System@TObject@$bdtr$qqrv
	dw	3
	dw	72
	db	83
	db	121
	db	115
	db	116
	db	101
	db	109
	db	58
	db	58
	db	84
	db	79
	db	98
	db	106
	db	101
	db	99
	db	116
	db	0
	dd	0
	dd	0
	dd	0
@$xt$14System@TObject	ends
_TEXT	ends
_DATA	segment dword public use32 'DATA'
s@	label	byte
	db	0,0,0
	align	4
_DATA	ends
_TEXT	segment dword public use32 'CODE'
_TEXT	ends
 ?debug  C FB040C40545669766154696D65724004000000
 ?debug  C FB041140247870243130545669766154696D657204000000
 ?debug  C FB0411404578746374726C73405454696D65724004000000
 ?debug  C FB0416402478702431354578746374726C73405454696D657204000000
 ?debug  C FB045140436C61737365734054436F6D706F6E656E744055706461746552656769737472792471717270313753797374656D40544D657461436C6173736F78313753797374656D40416E7369537472696E67743302000000
 ?debug  C FB041440436C61737365734054436F6D706F6E656E744004000000
	public	_hDlPortIO
	public	_lpDlReadPortUshort
	public	_lpDlWritePortUchar
	public	_lpDlWritePortUshort
@@ChipData@$bdtr$qv equ @ChipData@$bdtr$qv
 extrn   @ChipData@$bdtr$qv:near
 extrn   @$bdele$qpv:near
 extrn   __Exception_list:dword
 extrn   @__InitExceptBlockLDTC:near
@Sleep equ Sleep
 extrn   Sleep:near
 extrn   @$bnew$qui:near
@@ChipData@$bctr$qpc equ @ChipData@$bctr$qpc
 extrn   @ChipData@$bctr$qpc:near
	public	_CurrentBytePointer
	public	_CurrentBitPointer
@@VivaGlobals@SetProgPos$qii equ @VivaGlobals@SetProgPos$qii
 extrn   @VivaGlobals@SetProgPos$qii:near
@@ChipData@GetLength$qv equ @ChipData@GetLength$qv
 extrn   @ChipData@GetLength$qv:near
@@ChipData@GetDataBuffer$qv equ @ChipData@GetDataBuffer$qv
 extrn   @ChipData@GetDataBuffer$qv:near
@@Comctrls@TProgressBar@SetPosition$qqri equ @Comctrls@TProgressBar@SetPosition$qqri
 extrn   @Comctrls@TProgressBar@SetPosition$qqri:near
	public	_dseed
 extrn   @Extctrls@TTimer@:byte
@@Classes@TComponent@SafeCallException$qqrp14System@TObjectpv equ @Classes@TComponent@SafeCallException$qqrp14System@TObjectpv
 extrn   @Classes@TComponent@SafeCallException$qqrp14System@TObjectpv:near
@@System@TObject@AfterConstruction$qqrv equ @System@TObject@AfterConstruction$qqrv
 extrn   @System@TObject@AfterConstruction$qqrv:near
@@Classes@TComponent@BeforeDestruction$qqrv equ @Classes@TComponent@BeforeDestruction$qqrv
 extrn   @Classes@TComponent@BeforeDestruction$qqrv:near
@@System@TObject@Dispatch$qqrpv equ @System@TObject@Dispatch$qqrpv
 extrn   @System@TObject@Dispatch$qqrpv:near
@@System@TObject@DefaultHandler$qqrpv equ @System@TObject@DefaultHandler$qqrpv
 extrn   @System@TObject@DefaultHandler$qqrpv:near
@@System@TObject@NewInstance$qqrp17System@TMetaClass equ @System@TObject@NewInstance$qqrp17System@TMetaClass
 extrn   @System@TObject@NewInstance$qqrp17System@TMetaClass:near
@@System@TObject@FreeInstance$qqrv equ @System@TObject@FreeInstance$qqrv
 extrn   @System@TObject@FreeInstance$qqrv:near
@@Classes@TPersistent@AssignTo$qqrp19Classes@TPersistent equ @Classes@TPersistent@AssignTo$qqrp19Classes@TPersistent
 extrn   @Classes@TPersistent@AssignTo$qqrp19Classes@TPersistent:near
@@Classes@TComponent@DefineProperties$qqrp14Classes@TFiler equ @Classes@TComponent@DefineProperties$qqrp14Classes@TFiler
 extrn   @Classes@TComponent@DefineProperties$qqrp14Classes@TFiler:near
@@Classes@TPersistent@Assign$qqrp19Classes@TPersistent equ @Classes@TPersistent@Assign$qqrp19Classes@TPersistent
 extrn   @Classes@TPersistent@Assign$qqrp19Classes@TPersistent:near
@@Classes@TComponent@Loaded$qqrv equ @Classes@TComponent@Loaded$qqrv
 extrn   @Classes@TComponent@Loaded$qqrv:near
@@Classes@TComponent@Notification$qqrp18Classes@TComponent18Classes@TOperation equ @Classes@TComponent@Notification$qqrp18Classes@TComponent18Classes@TOperation
 extrn   @Classes@TComponent@Notification$qqrp18Classes@TComponent18Classes@TOperation:near
@@Classes@TComponent@ReadState$qqrp15Classes@TReader equ @Classes@TComponent@ReadState$qqrp15Classes@TReader
 extrn   @Classes@TComponent@ReadState$qqrp15Classes@TReader:near
@@Classes@TComponent@SetName$qqrx17System@AnsiString equ @Classes@TComponent@SetName$qqrx17System@AnsiString
 extrn   @Classes@TComponent@SetName$qqrx17System@AnsiString:near
@@Classes@TComponent@ValidateRename$qqrp18Classes@TComponentx17System@AnsiStringt2 equ @Classes@TComponent@ValidateRename$qqrp18Classes@TComponentx17System@AnsiStringt2
 extrn   @Classes@TComponent@ValidateRename$qqrp18Classes@TComponentx17System@AnsiStringt2:near
@@Classes@TComponent@WriteState$qqrp15Classes@TWriter equ @Classes@TComponent@WriteState$qqrp15Classes@TWriter
 extrn   @Classes@TComponent@WriteState$qqrp15Classes@TWriter:near
@@Classes@TComponent@QueryInterface$qqsrx5_GUIDpv equ @Classes@TComponent@QueryInterface$qqsrx5_GUIDpv
 extrn   @Classes@TComponent@QueryInterface$qqsrx5_GUIDpv:near
 extrn   @$xp$15Extctrls@TTimer:byte
@@Extctrls@TTimer@$bdtr$qqrv equ @Extctrls@TTimer@$bdtr$qqrv
 extrn   @Extctrls@TTimer@$bdtr$qqrv:near
 extrn   __BeforeDestruction:near
 extrn   __ClassDestroy:near
@@System@AnsiString@$bctr$qqrrx17System@AnsiString equ @System@AnsiString@$bctr$qqrrx17System@AnsiString
 extrn   @System@AnsiString@$bctr$qqrrx17System@AnsiString:near
@@Classes@TComponent@UpdateRegistry$qqrp17System@TMetaClassox17System@AnsiStringt3 equ @Classes@TComponent@UpdateRegistry$qqrp17System@TMetaClassox17System@AnsiStringt3
 extrn   @Classes@TComponent@UpdateRegistry$qqrp17System@TMetaClassox17System@AnsiStringt3:near
 extrn   @Classes@TComponent@:byte
@@System@AnsiString@$bdtr$qqrv equ @System@AnsiString@$bdtr$qqrv
 extrn   @System@AnsiString@$bdtr$qqrv:near
@@Extctrls@TTimer@$bctr$qqrp18Classes@TComponent equ @Extctrls@TTimer@$bctr$qqrp18Classes@TComponent
 extrn   @Extctrls@TTimer@$bctr$qqrp18Classes@TComponent:near
@@Extctrls@TTimer@SetOnTimer$qqrynpqqrp14System@TObject$v equ @Extctrls@TTimer@SetOnTimer$qqrynpqqrp14System@TObject$v
 extrn   @Extctrls@TTimer@SetOnTimer$qqrynpqqrp14System@TObject$v:near
@@TVivaTimer@TimerTimer$qqrp14System@TObject equ @TVivaTimer@TimerTimer$qqrp14System@TObject
 extrn   @TVivaTimer@TimerTimer$qqrp14System@TObject:near
 extrn   __ClassCreate:near
 extrn   __AfterConstruction:near
@@Classes@TComponent@$bdtr$qqrv equ @Classes@TComponent@$bdtr$qqrv
 extrn   @Classes@TComponent@$bdtr$qqrv:near
@@Classes@TPersistent@$bdtr$qqrv equ @Classes@TPersistent@$bdtr$qqrv
 extrn   @Classes@TPersistent@$bdtr$qqrv:near
@@System@TObject@$bdtr$qqrv equ @System@TObject@$bdtr$qqrv
 extrn   @System@TObject@$bdtr$qqrv:near
 ?debug  C FB0A41736D00
 ?debug  C 9F757569642E6C6962
 ?debug  C 9F757569642E6C6962
 ?debug  C 9F757569642E6C6962
 ?debug  C 9F757569642E6C6962
 ?debug  C 9F76636C6535302E6C6962
$$BSYMS	segment byte public use32 'DEBSYM'
	dw	22
	dw	514
	df	_hDlPortIO
	dw	0
	dw	1027
	dw	0
	dw	164
	dw	0
	dw	0
	dw	0
	dw	22
	dw	514
	df	_lpDlReadPortUshort
	dw	0
	dw	4264
	dw	0
	dw	165
	dw	0
	dw	0
	dw	0
	dw	22
	dw	514
	df	_lpDlWritePortUchar
	dw	0
	dw	4267
	dw	0
	dw	166
	dw	0
	dw	0
	dw	0
	dw	22
	dw	514
	df	_lpDlWritePortUshort
	dw	0
	dw	4270
	dw	0
	dw	167
	dw	0
	dw	0
	dw	0
	dw	22
	dw	514
	df	_CurrentBytePointer
	dw	0
	dw	116
	dw	0
	dw	168
	dw	0
	dw	0
	dw	0
	dw	22
	dw	514
	df	_CurrentBitPointer
	dw	0
	dw	116
	dw	0
	dw	169
	dw	0
	dw	0
	dw	0
	dw	22
	dw	514
	df	_dseed
	dw	0
	dw	116
	dw	0
	dw	170
	dw	0
	dw	0
	dw	0
	dw	?patch148
	dw	37
	dw	171
	dw	0
	dw	0
	dw	0
	dw	0
?patch148	equ	12
	dw	?patch149
	dw	37
	dw	172
	dw	0
	dw	0
	dw	0
	dw	0
?patch149	equ	12
	dw	?patch150
	dw	3
	dw	4261
	dw	0
	dw	173
	dw	0
	dw	0
	dw	0
	dw	2
?patch150	equ	16
	dw	?patch151
	dw	3
	dw	4261
	dw	0
	dw	174
	dw	0
	dw	0
	dw	0
	dw	3
?patch151	equ	16
	dw	?patch152
	dw	3
	dw	4261
	dw	0
	dw	175
	dw	0
	dw	0
	dw	0
	dw	4
?patch152	equ	16
	dw	?patch153
	dw	3
	dw	4261
	dw	0
	dw	176
	dw	0
	dw	0
	dw	0
	dw	5
?patch153	equ	16
	dw	?patch154
	dw	3
	dw	4261
	dw	0
	dw	177
	dw	0
	dw	0
	dw	0
	dw	6
?patch154	equ	16
	dw	?patch155
	dw	3
	dw	4261
	dw	0
	dw	178
	dw	0
	dw	0
	dw	0
	dw	7
?patch155	equ	16
	dw	?patch156
	dw	3
	dw	4261
	dw	0
	dw	179
	dw	0
	dw	0
	dw	0
	dw	8
?patch156	equ	16
	dw	?patch157
	dw	3
	dw	4261
	dw	0
	dw	180
	dw	0
	dw	0
	dw	0
	dw	9
?patch157	equ	16
	dw	?patch158
	dw	3
	dw	4261
	dw	0
	dw	181
	dw	0
	dw	0
	dw	0
	dw	11
?patch158	equ	16
	dw	?patch159
	dw	3
	dw	4261
	dw	0
	dw	182
	dw	0
	dw	0
	dw	0
	dw	12
?patch159	equ	16
	dw	?patch160
	dw	3
	dw	4261
	dw	0
	dw	183
	dw	0
	dw	0
	dw	0
	dw	13
?patch160	equ	16
	dw	?patch161
	dw	3
	dw	4261
	dw	0
	dw	184
	dw	0
	dw	0
	dw	0
	dw	17
?patch161	equ	16
	dw	?patch162
	dw	3
	dw	4262
	dw	0
	dw	185
	dw	0
	dw	0
	dw	0
	dw	16384
?patch162	equ	16
	dw	?patch163
	dw	3
	dw	4261
	dw	0
	dw	186
	dw	0
	dw	0
	dw	0
	dw	0
?patch163	equ	16
	dw	?patch164
	dw	3
	dw	4261
	dw	0
	dw	187
	dw	0
	dw	0
	dw	0
	dw	1
?patch164	equ	16
	dw	?patch165
	dw	3
	dw	4261
	dw	0
	dw	188
	dw	0
	dw	0
	dw	0
	dw	2
?patch165	equ	16
	dw	?patch166
	dw	3
	dw	4261
	dw	0
	dw	189
	dw	0
	dw	0
	dw	0
	dw	3
?patch166	equ	16
	dw	?patch167
	dw	3
	dw	4261
	dw	0
	dw	190
	dw	0
	dw	0
	dw	0
	dw	4
?patch167	equ	16
	dw	?patch168
	dw	3
	dw	4261
	dw	0
	dw	191
	dw	0
	dw	0
	dw	0
	dw	5
?patch168	equ	16
	dw	?patch169
	dw	3
	dw	4261
	dw	0
	dw	192
	dw	0
	dw	0
	dw	0
	dw	6
?patch169	equ	16
	dw	?patch170
	dw	3
	dw	4261
	dw	0
	dw	193
	dw	0
	dw	0
	dw	0
	dw	7
?patch170	equ	16
	dw	?patch171
	dw	3
	dw	4261
	dw	0
	dw	194
	dw	0
	dw	0
	dw	0
	dw	8
?patch171	equ	16
	dw	?patch172
	dw	3
	dw	4261
	dw	0
	dw	195
	dw	0
	dw	0
	dw	0
	dw	9
?patch172	equ	16
	dw	?patch173
	dw	3
	dw	4261
	dw	0
	dw	196
	dw	0
	dw	0
	dw	0
	dw	10
?patch173	equ	16
	dw	?patch174
	dw	3
	dw	4261
	dw	0
	dw	197
	dw	0
	dw	0
	dw	0
	dw	11
?patch174	equ	16
	dw	?patch175
	dw	3
	dw	4261
	dw	0
	dw	198
	dw	0
	dw	0
	dw	0
	dw	12
?patch175	equ	16
	dw	?patch176
	dw	3
	dw	4261
	dw	0
	dw	199
	dw	0
	dw	0
	dw	0
	dw	13
?patch176	equ	16
	dw	?patch177
	dw	37
	dw	200
	dw	0
	dw	0
	dw	0
	dw	0
?patch177	equ	12
	dw	?patch178
	dw	37
	dw	201
	dw	0
	dw	0
	dw	0
	dw	0
?patch178	equ	12
	dw	?patch179
	dw	37
	dw	202
	dw	0
	dw	0
	dw	0
	dw	0
?patch179	equ	12
	dw	?patch180
	dw	37
	dw	203
	dw	0
	dw	0
	dw	0
	dw	0
?patch180	equ	12
	dw	?patch181
	dw	37
	dw	204
	dw	0
	dw	0
	dw	0
	dw	0
?patch181	equ	12
	dw	?patch182
	dw	37
	dw	205
	dw	0
	dw	0
	dw	0
	dw	0
?patch182	equ	12
	dw	?patch183
	dw	37
	dw	206
	dw	0
	dw	0
	dw	0
	dw	0
?patch183	equ	12
	dw	?patch184
	dw	3
	dw	4263
	dw	0
	dw	207
	dw	0
	dw	0
	dw	0
	dw	0
?patch184	equ	16
	dw	?patch185
	dw	37
	dw	208
	dw	0
	dw	0
	dw	0
	dw	0
?patch185	equ	12
	dw	?patch186
	dw	37
	dw	209
	dw	0
	dw	0
	dw	0
	dw	0
?patch186	equ	12
	dw	?patch187
	dw	37
	dw	210
	dw	0
	dw	0
	dw	0
	dw	0
?patch187	equ	12
	dw	?patch188
	dw	37
	dw	211
	dw	0
	dw	0
	dw	0
	dw	0
?patch188	equ	12
	dw	?patch189
	dw	37
	dw	212
	dw	0
	dw	0
	dw	0
	dw	0
?patch189	equ	12
	dw	?patch190
	dw	37
	dw	213
	dw	0
	dw	0
	dw	0
	dw	0
?patch190	equ	12
	dw	?patch191
	dw	37
	dw	214
	dw	0
	dw	0
	dw	0
	dw	0
?patch191	equ	12
	dw	?patch192
	dw	37
	dw	215
	dw	0
	dw	0
	dw	0
	dw	0
?patch192	equ	12
	dw	?patch193
	dw	37
	dw	216
	dw	0
	dw	0
	dw	0
	dw	0
?patch193	equ	12
	dw	?patch194
	dw	37
	dw	217
	dw	0
	dw	0
	dw	0
	dw	0
?patch194	equ	12
	dw	?patch195
	dw	37
	dw	218
	dw	0
	dw	0
	dw	0
	dw	0
?patch195	equ	12
	dw	?patch196
	dw	37
	dw	219
	dw	0
	dw	0
	dw	0
	dw	0
?patch196	equ	12
	dw	?patch197
	dw	37
	dw	220
	dw	0
	dw	0
	dw	0
	dw	0
?patch197	equ	12
	dw	?patch198
	dw	37
	dw	221
	dw	0
	dw	0
	dw	0
	dw	0
?patch198	equ	12
	dw	?patch199
	dw	37
	dw	222
	dw	0
	dw	0
	dw	0
	dw	0
?patch199	equ	12
	dw	?patch200
	dw	37
	dw	223
	dw	0
	dw	0
	dw	0
	dw	0
?patch200	equ	12
	dw	?patch201
	dw	37
	dw	224
	dw	0
	dw	0
	dw	0
	dw	0
?patch201	equ	12
	dw	?patch202
	dw	37
	dw	225
	dw	0
	dw	0
	dw	0
	dw	0
?patch202	equ	12
	dw	?patch203
	dw	37
	dw	226
	dw	0
	dw	0
	dw	0
	dw	0
?patch203	equ	12
	dw	?patch204
	dw	37
	dw	227
	dw	0
	dw	0
	dw	0
	dw	0
?patch204	equ	12
	dw	?patch205
	dw	37
	dw	228
	dw	0
	dw	0
	dw	0
	dw	0
?patch205	equ	12
	dw	?patch206
	dw	37
	dw	229
	dw	0
	dw	0
	dw	0
	dw	0
?patch206	equ	12
	dw	?patch207
	dw	37
	dw	230
	dw	0
	dw	0
	dw	0
	dw	0
?patch207	equ	12
	dw	?patch208
	dw	37
	dw	231
	dw	0
	dw	0
	dw	0
	dw	0
?patch208	equ	12
	dw	?patch209
	dw	37
	dw	232
	dw	0
	dw	0
	dw	0
	dw	0
?patch209	equ	12
	dw	?patch210
	dw	37
	dw	233
	dw	0
	dw	0
	dw	0
	dw	0
?patch210	equ	12
	dw	?patch211
	dw	37
	dw	234
	dw	0
	dw	0
	dw	0
	dw	0
?patch211	equ	12
	dw	?patch212
	dw	37
	dw	235
	dw	0
	dw	0
	dw	0
	dw	0
?patch212	equ	12
	dw	?patch213
	dw	37
	dw	236
	dw	0
	dw	0
	dw	0
	dw	0
?patch213	equ	12
	dw	?patch214
	dw	37
	dw	237
	dw	0
	dw	0
	dw	0
	dw	0
?patch214	equ	12
	dw	?patch215
	dw	37
	dw	238
	dw	0
	dw	0
	dw	0
	dw	0
?patch215	equ	12
	dw	?patch216
	dw	37
	dw	239
	dw	0
	dw	0
	dw	0
	dw	0
?patch216	equ	12
	dw	?patch217
	dw	37
	dw	240
	dw	0
	dw	0
	dw	0
	dw	0
?patch217	equ	12
	dw	?patch218
	dw	37
	dw	241
	dw	0
	dw	0
	dw	0
	dw	0
?patch218	equ	12
	dw	?patch219
	dw	37
	dw	242
	dw	0
	dw	0
	dw	0
	dw	0
?patch219	equ	12
	dw	?patch220
	dw	37
	dw	243
	dw	0
	dw	0
	dw	0
	dw	0
?patch220	equ	12
	dw	?patch221
	dw	37
	dw	244
	dw	0
	dw	0
	dw	0
	dw	0
?patch221	equ	12
	dw	?patch222
	dw	37
	dw	245
	dw	0
	dw	0
	dw	0
	dw	0
?patch222	equ	12
	dw	?patch223
	dw	37
	dw	246
	dw	0
	dw	0
	dw	0
	dw	0
?patch223	equ	12
	dw	?patch224
	dw	37
	dw	247
	dw	0
	dw	0
	dw	0
	dw	0
?patch224	equ	12
	dw	?patch225
	dw	37
	dw	248
	dw	0
	dw	0
	dw	0
	dw	0
?patch225	equ	12
	dw	?patch226
	dw	37
	dw	249
	dw	0
	dw	0
	dw	0
	dw	0
?patch226	equ	12
	dw	?patch227
	dw	37
	dw	250
	dw	0
	dw	0
	dw	0
	dw	0
?patch227	equ	12
	dw	?patch228
	dw	37
	dw	251
	dw	0
	dw	0
	dw	0
	dw	0
?patch228	equ	12
	dw	?patch229
	dw	37
	dw	252
	dw	0
	dw	0
	dw	0
	dw	0
?patch229	equ	12
	dw	?patch230
	dw	37
	dw	253
	dw	0
	dw	0
	dw	0
	dw	0
?patch230	equ	12
	dw	?patch231
	dw	37
	dw	254
	dw	0
	dw	0
	dw	0
	dw	0
?patch231	equ	12
	dw	?patch232
	dw	37
	dw	255
	dw	0
	dw	0
	dw	0
	dw	0
?patch232	equ	12
	dw	?patch233
	dw	37
	dw	256
	dw	0
	dw	0
	dw	0
	dw	0
?patch233	equ	12
	dw	?patch234
	dw	37
	dw	257
	dw	0
	dw	0
	dw	0
	dw	0
?patch234	equ	12
	dw	?patch235
	dw	37
	dw	258
	dw	0
	dw	0
	dw	0
	dw	0
?patch235	equ	12
	dw	?patch236
	dw	37
	dw	259
	dw	0
	dw	0
	dw	0
	dw	0
?patch236	equ	12
	dw	?patch237
	dw	37
	dw	260
	dw	0
	dw	0
	dw	0
	dw	0
?patch237	equ	12
	dw	?patch238
	dw	37
	dw	261
	dw	0
	dw	0
	dw	0
	dw	0
?patch238	equ	12
	dw	?patch239
	dw	37
	dw	262
	dw	0
	dw	0
	dw	0
	dw	0
?patch239	equ	12
	dw	?patch240
	dw	37
	dw	263
	dw	0
	dw	0
	dw	0
	dw	0
?patch240	equ	12
	dw	?patch241
	dw	37
	dw	264
	dw	0
	dw	0
	dw	0
	dw	0
?patch241	equ	12
	dw	?patch242
	dw	37
	dw	265
	dw	0
	dw	0
	dw	0
	dw	0
?patch242	equ	12
	dw	?patch243
	dw	37
	dw	266
	dw	0
	dw	0
	dw	0
	dw	0
?patch243	equ	12
	dw	?patch244
	dw	37
	dw	267
	dw	0
	dw	0
	dw	0
	dw	0
?patch244	equ	12
	dw	?patch245
	dw	37
	dw	268
	dw	0
	dw	0
	dw	0
	dw	0
?patch245	equ	12
	dw	?patch246
	dw	37
	dw	269
	dw	0
	dw	0
	dw	0
	dw	0
?patch246	equ	12
	dw	?patch247
	dw	37
	dw	270
	dw	0
	dw	0
	dw	0
	dw	0
?patch247	equ	12
	dw	?patch248
	dw	37
	dw	271
	dw	0
	dw	0
	dw	0
	dw	0
?patch248	equ	12
	dw	16
	dw	4
	dw	4264
	dw	0
	dw	0
	dw	272
	dw	0
	dw	0
	dw	0
	dw	16
	dw	4
	dw	4267
	dw	0
	dw	0
	dw	273
	dw	0
	dw	0
	dw	0
	dw	16
	dw	4
	dw	4270
	dw	0
	dw	0
	dw	274
	dw	0
	dw	0
	dw	0
	dw	16
	dw	4
	dw	4106
	dw	0
	dw	1
	dw	275
	dw	0
	dw	0
	dw	0
	dw	16
	dw	4
	dw	4231
	dw	0
	dw	1
	dw	276
	dw	0
	dw	0
	dw	0
	dw	?patch249
	dw	37
	dw	277
	dw	0
	dw	0
	dw	0
	dw	0
?patch249	equ	12
	dw	?patch250
	dw	3
	dw	4273
	dw	0
	dw	278
	dw	0
	dw	0
	dw	0
	dw	54
?patch250	equ	16
	dw	16
	dw	4
	dw	4103
	dw	0
	dw	1
	dw	279
	dw	0
	dw	0
	dw	0
	dw	16
	dw	4
	dw	18
	dw	0
	dw	0
	dw	280
	dw	0
	dw	0
	dw	0
	dw	16
	dw	4
	dw	18
	dw	0
	dw	0
	dw	281
	dw	0
	dw	0
	dw	0
	dw	?patch251
	dw	38
	dw	436
	dw	282
	dw	0
	dw	283
	dw	0
	dw	284
	dw	0
	dw	285
	dw	0
	dw	286
	dw	0
	dw	287
	dw	0
	dw	288
	dw	0
	dw	289
	dw	0
	dw	290
	dw	0
	dw	291
	dw	0
	dw	292
	dw	0
	dw	293
	dw	0
	dw	294
	dw	0
	dw	295
	dw	0
	dw	296
	dw	0
	dw	297
	dw	0
	dw	298
	dw	0
	dw	299
	dw	0
	dw	300
	dw	0
	dw	301
	dw	0
	dw	302
	dw	0
	dw	303
	dw	0
	dw	304
	dw	0
	dw	305
	dw	0
	dw	306
	dw	0
	dw	307
	dw	0
	dw	308
	dw	0
	dw	309
	dw	0
	dw	310
	dw	0
	dw	311
	dw	0
	dw	312
	dw	0
	dw	313
	dw	0
	dw	314
	dw	0
	dw	315
	dw	0
	dw	316
	dw	0
	dw	317
	dw	0
	dw	318
	dw	0
	dw	319
	dw	0
	dw	320
	dw	0
	dw	321
	dw	0
	dw	322
	dw	0
	dw	323
	dw	0
	dw	324
	dw	0
	dw	325
	dw	0
	dw	326
	dw	0
	dw	327
	dw	0
	dw	328
	dw	0
	dw	329
	dw	0
	dw	330
	dw	0
	dw	331
	dw	0
	dw	332
	dw	0
	dw	333
	dw	0
	dw	334
	dw	0
	dw	335
	dw	0
	dw	336
	dw	0
	dw	337
	dw	0
	dw	338
	dw	0
	dw	339
	dw	0
	dw	340
	dw	0
	dw	341
	dw	0
	dw	342
	dw	0
	dw	343
	dw	0
	dw	344
	dw	0
	dw	345
	dw	0
	dw	346
	dw	0
	dw	347
	dw	0
	dw	348
	dw	0
	dw	349
	dw	0
	dw	350
	dw	0
	dw	351
	dw	0
	dw	352
	dw	0
	dw	353
	dw	0
	dw	354
	dw	0
	dw	355
	dw	0
	dw	356
	dw	0
	dw	357
	dw	0
	dw	358
	dw	0
	dw	359
	dw	0
	dw	360
	dw	0
	dw	361
	dw	0
	dw	362
	dw	0
	dw	363
	dw	0
	dw	364
	dw	0
	dw	365
	dw	0
	dw	366
	dw	0
	dw	367
	dw	0
	dw	368
	dw	0
	dw	369
	dw	0
	dw	370
	dw	0
	dw	371
	dw	0
	dw	372
	dw	0
	dw	373
	dw	0
	dw	374
	dw	0
	dw	375
	dw	0
	dw	376
	dw	0
	dw	377
	dw	0
	dw	378
	dw	0
	dw	379
	dw	0
	dw	380
	dw	0
	dw	381
	dw	0
	dw	382
	dw	0
	dw	383
	dw	0
	dw	384
	dw	0
	dw	385
	dw	0
	dw	386
	dw	0
	dw	387
	dw	0
	dw	388
	dw	0
	dw	389
	dw	0
	dw	390
	dw	0
	dw	391
	dw	0
	dw	392
	dw	0
	dw	393
	dw	0
	dw	394
	dw	0
	dw	395
	dw	0
	dw	396
	dw	0
	dw	397
	dw	0
	dw	398
	dw	0
	dw	399
	dw	0
	dw	400
	dw	0
	dw	401
	dw	0
	dw	402
	dw	0
	dw	403
	dw	0
	dw	404
	dw	0
	dw	405
	dw	0
	dw	406
	dw	0
	dw	407
	dw	0
	dw	408
	dw	0
	dw	409
	dw	0
	dw	410
	dw	0
	dw	411
	dw	0
	dw	412
	dw	0
	dw	413
	dw	0
	dw	414
	dw	0
	dw	415
	dw	0
	dw	416
	dw	0
	dw	417
	dw	0
	dw	418
	dw	0
	dw	419
	dw	0
	dw	420
	dw	0
	dw	421
	dw	0
	dw	422
	dw	0
	dw	423
	dw	0
	dw	424
	dw	0
	dw	425
	dw	0
	dw	426
	dw	0
	dw	427
	dw	0
	dw	428
	dw	0
	dw	429
	dw	0
	dw	430
	dw	0
	dw	431
	dw	0
	dw	432
	dw	0
	dw	433
	dw	0
	dw	434
	dw	0
	dw	435
	dw	0
	dw	436
	dw	0
	dw	437
	dw	0
	dw	438
	dw	0
	dw	439
	dw	0
	dw	440
	dw	0
	dw	441
	dw	0
	dw	442
	dw	0
	dw	443
	dw	0
	dw	444
	dw	0
	dw	445
	dw	0
	dw	446
	dw	0
	dw	447
	dw	0
	dw	448
	dw	0
	dw	449
	dw	0
	dw	450
	dw	0
	dw	451
	dw	0
	dw	452
	dw	0
	dw	453
	dw	0
	dw	454
	dw	0
	dw	455
	dw	0
	dw	456
	dw	0
	dw	457
	dw	0
	dw	458
	dw	0
	dw	459
	dw	0
	dw	460
	dw	0
	dw	461
	dw	0
	dw	462
	dw	0
	dw	463
	dw	0
	dw	464
	dw	0
	dw	465
	dw	0
	dw	466
	dw	0
	dw	467
	dw	0
	dw	468
	dw	0
	dw	469
	dw	0
	dw	470
	dw	0
	dw	471
	dw	0
	dw	472
	dw	0
	dw	473
	dw	0
	dw	474
	dw	0
	dw	475
	dw	0
	dw	476
	dw	0
	dw	477
	dw	0
	dw	478
	dw	0
	dw	479
	dw	0
	dw	480
	dw	0
	dw	481
	dw	0
	dw	482
	dw	0
	dw	483
	dw	0
	dw	484
	dw	0
	dw	485
	dw	0
	dw	486
	dw	0
	dw	487
	dw	0
	dw	488
	dw	0
	dw	489
	dw	0
	dw	490
	dw	0
	dw	491
	dw	0
	dw	492
	dw	0
	dw	493
	dw	0
	dw	494
	dw	0
	dw	495
	dw	0
	dw	496
	dw	0
	dw	497
	dw	0
	dw	498
	dw	0
	dw	499
	dw	0
	dw	500
	dw	0
	dw	501
	dw	0
	dw	502
	dw	0
	dw	503
	dw	0
	dw	504
	dw	0
	dw	505
	dw	0
	dw	506
	dw	0
	dw	507
	dw	0
	dw	508
	dw	0
	dw	509
	dw	0
	dw	510
	dw	0
	dw	511
	dw	0
	dw	512
	dw	0
	dw	513
	dw	0
	dw	514
	dw	0
	dw	515
	dw	0
	dw	516
	dw	0
	dw	517
	dw	0
	dw	518
	dw	0
	dw	519
	dw	0
	dw	520
	dw	0
	dw	521
	dw	0
	dw	522
	dw	0
	dw	523
	dw	0
	dw	524
	dw	0
	dw	525
	dw	0
	dw	526
	dw	0
	dw	527
	dw	0
	dw	528
	dw	0
	dw	529
	dw	0
	dw	530
	dw	0
	dw	531
	dw	0
	dw	532
	dw	0
	dw	533
	dw	0
	dw	534
	dw	0
	dw	535
	dw	0
	dw	536
	dw	0
	dw	537
	dw	0
	dw	538
	dw	0
	dw	539
	dw	0
	dw	540
	dw	0
	dw	541
	dw	0
	dw	542
	dw	0
	dw	543
	dw	0
	dw	544
	dw	0
	dw	545
	dw	0
	dw	546
	dw	0
	dw	547
	dw	0
	dw	548
	dw	0
	dw	549
	dw	0
	dw	550
	dw	0
	dw	551
	dw	0
	dw	552
	dw	0
	dw	553
	dw	0
	dw	554
	dw	0
	dw	555
	dw	0
	dw	556
	dw	0
	dw	557
	dw	0
	dw	558
	dw	0
	dw	559
	dw	0
	dw	560
	dw	0
	dw	561
	dw	0
	dw	562
	dw	0
	dw	563
	dw	0
	dw	564
	dw	0
	dw	565
	dw	0
	dw	566
	dw	0
	dw	567
	dw	0
	dw	568
	dw	0
	dw	569
	dw	0
	dw	570
	dw	0
	dw	571
	dw	0
	dw	572
	dw	0
	dw	573
	dw	0
	dw	574
	dw	0
	dw	575
	dw	0
	dw	576
	dw	0
	dw	577
	dw	0
	dw	578
	dw	0
	dw	579
	dw	0
	dw	580
	dw	0
	dw	581
	dw	0
	dw	582
	dw	0
	dw	583
	dw	0
	dw	584
	dw	0
	dw	585
	dw	0
	dw	586
	dw	0
	dw	587
	dw	0
	dw	588
	dw	0
	dw	589
	dw	0
	dw	590
	dw	0
	dw	591
	dw	0
	dw	592
	dw	0
	dw	593
	dw	0
	dw	594
	dw	0
	dw	595
	dw	0
	dw	596
	dw	0
	dw	597
	dw	0
	dw	598
	dw	0
	dw	599
	dw	0
	dw	600
	dw	0
	dw	601
	dw	0
	dw	602
	dw	0
	dw	603
	dw	0
	dw	604
	dw	0
	dw	605
	dw	0
	dw	606
	dw	0
	dw	607
	dw	0
	dw	608
	dw	0
	dw	609
	dw	0
	dw	610
	dw	0
	dw	611
	dw	0
	dw	612
	dw	0
	dw	613
	dw	0
	dw	614
	dw	0
	dw	615
	dw	0
	dw	616
	dw	0
	dw	617
	dw	0
	dw	618
	dw	0
	dw	619
	dw	0
	dw	620
	dw	0
	dw	621
	dw	0
	dw	622
	dw	0
	dw	623
	dw	0
	dw	624
	dw	0
	dw	625
	dw	0
	dw	626
	dw	0
	dw	627
	dw	0
	dw	628
	dw	0
	dw	629
	dw	0
	dw	630
	dw	0
	dw	631
	dw	0
	dw	632
	dw	0
	dw	633
	dw	0
	dw	634
	dw	0
	dw	635
	dw	0
	dw	636
	dw	0
	dw	637
	dw	0
	dw	638
	dw	0
	dw	639
	dw	0
	dw	640
	dw	0
	dw	641
	dw	0
	dw	642
	dw	0
	dw	643
	dw	0
	dw	644
	dw	0
	dw	645
	dw	0
	dw	646
	dw	0
	dw	647
	dw	0
	dw	648
	dw	0
	dw	649
	dw	0
	dw	650
	dw	0
	dw	651
	dw	0
	dw	652
	dw	0
	dw	653
	dw	0
	dw	654
	dw	0
	dw	655
	dw	0
	dw	656
	dw	0
	dw	657
	dw	0
	dw	658
	dw	0
	dw	659
	dw	0
	dw	660
	dw	0
	dw	661
	dw	0
	dw	662
	dw	0
	dw	663
	dw	0
	dw	664
	dw	0
	dw	665
	dw	0
	dw	666
	dw	0
	dw	667
	dw	0
	dw	668
	dw	0
	dw	669
	dw	0
	dw	670
	dw	0
	dw	671
	dw	0
	dw	672
	dw	0
	dw	673
	dw	0
	dw	674
	dw	0
	dw	675
	dw	0
	dw	676
	dw	0
	dw	677
	dw	0
	dw	678
	dw	0
	dw	679
	dw	0
	dw	680
	dw	0
	dw	681
	dw	0
	dw	682
	dw	0
	dw	683
	dw	0
	dw	684
	dw	0
	dw	685
	dw	0
	dw	686
	dw	0
	dw	687
	dw	0
	dw	688
	dw	0
	dw	689
	dw	0
	dw	690
	dw	0
	dw	691
	dw	0
	dw	692
	dw	0
	dw	693
	dw	0
	dw	694
	dw	0
	dw	695
	dw	0
	dw	696
	dw	0
	dw	697
	dw	0
	dw	698
	dw	0
	dw	699
	dw	0
	dw	700
	dw	0
	dw	701
	dw	0
	dw	702
	dw	0
	dw	703
	dw	0
	dw	704
	dw	0
	dw	705
	dw	0
	dw	706
	dw	0
	dw	707
	dw	0
	dw	708
	dw	0
	dw	709
	dw	0
	dw	710
	dw	0
	dw	711
	dw	0
	dw	712
	dw	0
	dw	713
	dw	0
	dw	714
	dw	0
	dw	715
	dw	0
	dw	716
	dw	0
	dw	717
	dw	0
?patch251	equ	1748
	dw	?patch252
	dw	1
	db	5
	db	1
	db	0
	db	24
	db	9
	db	66
	db	67
	db	67
	db	51
	db	50
	db	32
	db	53
	db	46
	db	53
?patch252	equ	16
$$BSYMS	ends
$$BTYPES	segment byte public use32 'DEBTYP'
	db        2,0,0,0,14,0,8,0,3,0,0,0,4,0,2,0
	db        1,16,0,0,12,0,1,2,2,0,33,0,0,0,32,0
	db        0,0,14,0,8,0,3,0,0,0,4,0,2,0,3,16
	db        0,0,12,0,1,2,2,0,33,0,0,0,33,0,0,0
	db        14,0,8,0,33,0,0,0,4,0,1,0,5,16,0,0
	db        8,0,1,2,1,0,33,0,0,0,26,0,9,0,3,0
	db        0,0,7,16,0,0,33,16,0,0,0,0,0,0,93,16
	db        0,0,0,0,0,0,28,0,4,0,22,0,92,16,0,0
	db        2,1,0,0,0,0,0,0,0,0,0,0,0,0,9,0
	db        0,0,46,0,18,0,3,0,9,16,0,0,17,0,0,0
	db        0,0,0,0,40,0,10,0,8,0,2,0,10,0,10,16
	db        0,0,28,0,4,0,8,0,31,16,0,0,2,1,0,0
	db        0,0,0,0,0,0,0,0,0,0,10,0,0,0,12,0
	db        26,0,9,0,9,16,0,0,10,16,0,0,12,16,0,0
	db        0,0,1,0,13,16,0,0,0,0,0,0,8,0,2,0
	db        10,4,10,16,0,0,8,0,1,2,1,0,112,4,0,0
	db        26,0,9,0,9,16,0,0,10,16,0,0,12,16,0,0
	db        0,0,1,0,17,16,0,0,0,0,0,0,8,0,2,0
	db        42,0,16,16,0,0,8,0,1,0,1,0,10,16,0,0
	db        8,0,1,2,1,0,15,16,0,0,26,0,9,0,3,0
	db        0,0,10,16,0,0,12,16,0,0,0,0,0,0,19,16
	db        0,0,0,0,0,0,4,0,1,2,0,0,26,0,9,0
	db        112,4,0,0,10,16,0,0,12,16,0,0,0,0,0,0
	db        21,16,0,0,0,0,0,0,4,0,1,2,0,0,26,0
	db        9,0,116,0,0,0,10,16,0,0,12,16,0,0,0,0
	db        0,0,23,16,0,0,0,0,0,0,4,0,1,2,0,0
	db        26,0,9,0,116,0,0,0,10,16,0,0,12,16,0,0
	db        0,0,0,0,25,16,0,0,0,0,0,0,4,0,1,2
	db        0,0,22,0,7,2,3,4,11,16,0,0,0,0,0,0
	db        35,20,14,16,0,0,0,0,0,0,12,0,7,2,3,8
	db        18,16,0,0,0,0,0,0,12,0,7,2,3,0,20,16
	db        0,0,0,0,0,0,12,0,7,2,3,0,22,16,0,0
	db        0,0,0,0,12,0,7,2,3,0,24,16,0,0,0,0
	db        0,0,122,0,4,2,6,4,112,4,0,0,3,0,11,0
	db        0,0,0,0,0,0,0,0,242,241,6,4,116,0,0,0
	db        3,0,12,0,0,0,0,0,0,0,4,0,242,241,6,4
	db        116,0,0,0,3,0,13,0,0,0,0,0,0,0,8,0
	db        242,241,8,4,2,0,26,16,0,0,14,0,0,0,8,4
	db        1,0,27,16,0,0,15,0,0,0,8,4,1,0,28,16
	db        0,0,16,0,0,0,8,4,1,0,29,16,0,0,17,0
	db        0,0,8,4,1,0,30,16,0,0,18,0,0,0,26,0
	db        9,0,3,0,0,0,7,16,0,0,33,16,0,0,0,0
	db        0,0,34,16,0,0,0,0,0,0,8,0,2,0,10,4
	db        7,16,0,0,4,0,1,2,0,0,26,0,9,0,3,0
	db        0,0,7,16,0,0,33,16,0,0,0,0,2,0,36,16
	db        0,0,0,0,0,0,12,0,1,2,2,0,33,0,0,0
	db        33,0,0,0,26,0,9,0,33,0,0,0,7,16,0,0
	db        33,16,0,0,0,0,0,0,38,16,0,0,0,0,0,0
	db        4,0,1,2,0,0,26,0,9,0,33,0,0,0,7,16
	db        0,0,33,16,0,0,0,0,0,0,40,16,0,0,0,0
	db        0,0,4,0,1,2,0,0,26,0,9,0,18,0,0,0
	db        7,16,0,0,33,16,0,0,0,0,0,0,42,16,0,0
	db        0,0,0,0,4,0,1,2,0,0,26,0,9,0,48,0
	db        0,0,7,16,0,0,33,16,0,0,0,0,1,0,44,16
	db        0,0,0,0,0,0,8,0,1,2,1,0,116,0,0,0
	db        26,0,9,0,3,0,0,0,7,16,0,0,33,16,0,0
	db        4,0,3,0,46,16,0,0,0,0,0,0,16,0,1,2
	db        3,0,33,0,0,0,112,4,0,0,18,0,0,0,26,0
	db        9,0,3,0,0,0,7,16,0,0,33,16,0,0,4,0
	db        3,0,48,16,0,0,0,0,0,0,16,0,1,2,3,0
	db        33,0,0,0,112,4,0,0,18,0,0,0,26,0,9,0
	db        3,0,0,0,7,16,0,0,33,16,0,0,4,0,4,0
	db        50,16,0,0,0,0,0,0,20,0,1,2,4,0,33,0
	db        0,0,112,4,0,0,112,4,0,0,18,0,0,0,26,0
	db        9,0,52,16,0,0,7,16,0,0,33,16,0,0,0,0
	db        0,0,53,16,0,0,0,0,0,0,8,0,2,0,10,0
	db        7,16,0,0,4,0,1,2,0,0,26,0,9,0,52,16
	db        0,0,7,16,0,0,33,16,0,0,0,0,1,0,57,16
	db        0,0,0,0,0,0,8,0,2,0,42,0,56,16,0,0
	db        8,0,1,0,1,0,7,16,0,0,8,0,1,2,1,0
	db        55,16,0,0,26,0,9,0,3,0,0,0,7,16,0,0
	db        33,16,0,0,0,0,0,0,59,16,0,0,0,0,0,0
	db        4,0,1,2,0,0,26,0,9,0,3,0,0,0,7,16
	db        0,0,33,16,0,0,0,0,2,0,61,16,0,0,0,0
	db        0,0,12,0,1,2,2,0,33,0,0,0,33,0,0,0
	db        26,0,9,0,33,0,0,0,7,16,0,0,33,16,0,0
	db        0,0,1,0,63,16,0,0,0,0,0,0,8,0,1,2
	db        1,0,33,0,0,0,26,0,9,0,33,0,0,0,7,16
	db        0,0,33,16,0,0,0,0,2,0,65,16,0,0,0,0
	db        0,0,12,0,1,2,2,0,116,0,0,0,112,4,0,0
	db        26,0,9,0,34,0,0,0,7,16,0,0,33,16,0,0
	db        4,0,1,0,67,16,0,0,0,0,0,0,8,0,1,2
	db        1,0,34,0,0,0,26,0,9,0,3,0,0,0,7,16
	db        0,0,33,16,0,0,4,0,1,0,69,16,0,0,0,0
	db        0,0,8,0,1,2,1,0,33,0,0,0,26,0,9,0
	db        3,0,0,0,7,16,0,0,33,16,0,0,4,0,4,0
	db        71,16,0,0,0,0,0,0,20,0,1,2,4,0,33,0
	db        0,0,112,4,0,0,255,255,255,255,18,0,0,0,26,0
	db        9,0,3,0,0,0,7,16,0,0,33,16,0,0,4,0
	db        4,0,73,16,0,0,0,0,0,0,20,0,1,2,4,0
	db        33,0,0,0,112,4,0,0,255,255,255,255,18,0,0,0
	db        12,0,7,2,1,0,32,16,0,0,0,0,0,0,12,0
	db        7,2,1,0,35,16,0,0,0,0,0,0,12,0,7,2
	db        1,0,37,16,0,0,0,0,0,0,12,0,7,2,1,0
	db        39,16,0,0,0,0,0,0,12,0,7,2,1,0,41,16
	db        0,0,0,0,0,0,12,0,7,2,1,0,43,16,0,0
	db        0,0,0,0,12,0,7,2,1,0,45,16,0,0,0,0
	db        0,0,12,0,7,2,1,0,47,16,0,0,0,0,0,0
	db        12,0,7,2,1,0,49,16,0,0,0,0,0,0,22,0
	db        7,2,3,4,51,16,0,0,0,0,0,0,35,20,54,16
	db        0,0,0,0,0,0,12,0,7,2,3,8,58,16,0,0
	db        0,0,0,0,12,0,7,2,3,0,60,16,0,0,0,0
	db        0,0,12,0,7,2,3,0,62,16,0,0,0,0,0,0
	db        12,0,7,2,3,0,64,16,0,0,0,0,0,0,12,0
	db        7,2,3,0,66,16,0,0,0,0,0,0,12,0,7,2
	db        3,0,68,16,0,0,0,0,0,0,12,0,7,2,3,0
	db        70,16,0,0,0,0,0,0,12,0,7,2,3,0,72,16
	db        0,0,0,0,0,0,42,1,4,2,6,4,8,16,0,0
	db        1,0,19,0,0,0,0,0,0,0,0,0,242,241,6,4
	db        33,0,0,0,1,0,20,0,0,0,0,0,0,0,40,0
	db        242,241,6,4,33,0,0,0,1,0,21,0,0,0,0,0
	db        0,0,42,0,242,241,6,4,33,0,0,0,1,0,22,0
	db        0,0,0,0,0,0,44,0,242,241,8,4,1,0,74,16
	db        0,0,23,0,0,0,8,4,1,0,75,16,0,0,24,0
	db        0,0,8,4,1,0,76,16,0,0,25,0,0,0,8,4
	db        1,0,77,16,0,0,26,0,0,0,8,4,1,0,78,16
	db        0,0,27,0,0,0,8,4,1,0,79,16,0,0,28,0
	db        0,0,8,4,1,0,80,16,0,0,29,0,0,0,8,4
	db        1,0,81,16,0,0,30,0,0,0,8,4,1,0,82,16
	db        0,0,31,0,0,0,8,4,2,0,83,16,0,0,32,0
	db        0,0,8,4,1,0,84,16,0,0,33,0,0,0,8,4
	db        1,0,85,16,0,0,34,0,0,0,8,4,1,0,86,16
	db        0,0,35,0,0,0,8,4,1,0,87,16,0,0,36,0
	db        0,0,8,4,1,0,88,16,0,0,37,0,0,0,8,4
	db        1,0,89,16,0,0,38,0,0,0,8,4,1,0,90,16
	db        0,0,39,0,0,0,8,4,1,0,91,16,0,0,40,0
	db        0,0,4,0,1,2,0,0,26,0,9,0,3,0,0,0
	db        7,16,0,0,33,16,0,0,0,0,2,0,95,16,0,0
	db        0,0,0,0,12,0,1,2,2,0,33,0,0,0,33,0
	db        0,0,26,0,9,0,34,0,0,0,7,16,0,0,33,16
	db        0,0,4,0,1,0,97,16,0,0,0,0,0,0,8,0
	db        1,2,1,0,34,0,0,0,26,0,9,0,33,0,0,0
	db        7,16,0,0,33,16,0,0,0,0,2,0,99,16,0,0
	db        0,0,0,0,12,0,1,2,2,0,116,0,0,0,112,4
	db        0,0,26,0,9,0,3,0,0,0,7,16,0,0,33,16
	db        0,0,0,0,0,0,101,16,0,0,0,0,0,0,4,0
	db        1,2,0,0,26,0,9,0,3,0,0,0,7,16,0,0
	db        33,16,0,0,0,0,2,0,103,16,0,0,0,0,0,0
	db        12,0,1,2,2,0,33,0,0,0,33,0,0,0,26,0
	db        9,0,33,0,0,0,7,16,0,0,33,16,0,0,0,0
	db        0,0,105,16,0,0,0,0,0,0,4,0,1,2,0,0
	db        26,0,9,0,33,0,0,0,7,16,0,0,33,16,0,0
	db        0,0,0,0,107,16,0,0,0,0,0,0,4,0,1,2
	db        0,0,26,0,9,0,18,0,0,0,7,16,0,0,33,16
	db        0,0,0,0,0,0,109,16,0,0,0,0,0,0,4,0
	db        1,2,0,0,26,0,9,0,48,0,0,0,7,16,0,0
	db        33,16,0,0,0,0,1,0,111,16,0,0,0,0,0,0
	db        8,0,1,2,1,0,116,0,0,0,26,0,9,0,33,0
	db        0,0,7,16,0,0,33,16,0,0,0,0,1,0,113,16
	db        0,0,0,0,0,0,8,0,1,2,1,0,33,0,0,0
	db        26,0,9,0,3,0,0,0,7,16,0,0,33,16,0,0
	db        4,0,3,0,115,16,0,0,0,0,0,0,16,0,1,2
	db        3,0,33,0,0,0,112,4,0,0,18,0,0,0,26,0
	db        9,0,3,0,0,0,7,16,0,0,33,16,0,0,4,0
	db        4,0,117,16,0,0,0,0,0,0,20,0,1,2,4,0
	db        33,0,0,0,112,4,0,0,112,4,0,0,18,0,0,0
	db        26,0,9,0,3,0,0,0,7,16,0,0,33,16,0,0
	db        4,0,3,0,119,16,0,0,0,0,0,0,16,0,1,2
	db        3,0,33,0,0,0,112,4,0,0,18,0,0,0,26,0
	db        9,0,3,0,0,0,7,16,0,0,33,16,0,0,4,0
	db        1,0,121,16,0,0,0,0,0,0,8,0,1,2,1,0
	db        33,0,0,0,26,0,9,0,3,0,0,0,7,16,0,0
	db        33,16,0,0,4,0,4,0,123,16,0,0,0,0,0,0
	db        20,0,1,2,4,0,33,0,0,0,112,4,0,0,255,255
	db        255,255,18,0,0,0,26,0,9,0,3,0,0,0,7,16
	db        0,0,33,16,0,0,4,0,4,0,125,16,0,0,0,0
	db        0,0,20,0,1,2,4,0,33,0,0,0,112,4,0,0
	db        255,255,255,255,18,0,0,0,14,0,8,0,116,0,0,0
	db        0,0,0,0,127,16,0,0,4,0,1,2,0,0,14,0
	db        8,0,116,0,0,0,0,0,1,0,129,16,0,0,8,0
	db        1,2,1,0,116,0,0,0,14,0,8,0,3,0,0,0
	db        0,0,0,0,131,16,0,0,4,0,1,2,0,0,26,0
	db        9,0,116,0,0,0,10,16,0,0,12,16,0,0,0,0
	db        0,0,133,16,0,0,0,0,0,0,4,0,1,2,0,0
	db        26,0,9,0,3,0,0,0,135,16,0,0,140,16,0,0
	db        4,0,0,0,158,16,0,0,0,0,0,0,28,0,4,0
	db        6,0,157,16,0,0,34,1,0,0,0,0,0,0,0,0
	db        136,16,0,0,149,0,0,0,64,0,5,0,10,0,2,0
	db        85,8,0,2,0,10,0,136,16,0,0,26,0,9,0,139
	db        16,0,0,135,16,0,0,140,16,0,0,4,0,1,0,141
	db        16,0,0,0,0,0,0,8,0,2,0,10,0,135,16,0
	db        0,8,0,2,0,10,4,135,16,0,0,8,0,1,2,1
	db        0,253,255,255,255,26,0,9,0,139,16,0,0,135,16,0
	db        0,140,16,0,0,0,0,1,0,145,16,0,0,0,0,0
	db        0,8,0,2,0,42,0,144,16,0,0,8,0,1,0,1
	db        0,135,16,0,0,8,0,1,2,1,0,143,16,0,0,26
	db        0,9,0,3,0,0,0,135,16,0,0,140,16,0,0,4
	db        0,1,0,147,16,0,0,0,0,0,0,8,0,1,2,1
	db        0,252,255,255,255,26,0,9,0,149,16,0,0,135,16,0
	db        0,140,16,0,0,0,0,1,0,150,16,0,0,0,0,0
	db        0,8,0,2,0,42,0,135,16,0,0,8,0,1,2,1
	db        0,149,16,0,0,26,0,9,0,3,0,0,0,135,16,0
	db        0,140,16,0,0,4,0,0,0,152,16,0,0,0,0,0
	db        0,4,0,1,2,0,0,26,0,7,2,19,4,138,16,0
	db        0,0,0,0,0,44,0,0,0,35,20,142,16,0,0,0
	db        0,0,0,12,0,7,2,3,0,146,16,0,0,0,0,0
	db        0,12,0,7,2,35,17,148,16,0,0,0,0,0,0,16
	db        0,7,2,51,24,151,16,0,0,0,0,0,0,252,255,255
	db        255,70,0,4,2,10,4,137,16,0,0,0,0,0,4,254
	db        255,255,255,3,0,0,0,242,241,8,4,2,0,153,16,0
	db        0,150,0,0,0,8,4,1,0,154,16,0,0,151,0,0
	db        0,8,4,1,0,155,16,0,0,152,0,0,0,8,4,1
	db        0,156,16,0,0,153,0,0,0,4,0,1,2,0,0,26
	db        0,9,0,3,0,0,0,251,255,255,255,160,16,0,0,4
	db        0,3,0,162,16,0,0,0,0,0,0,8,0,2,0,10
	db        4,251,255,255,255,8,0,1,0,1,0,250,255,255,255,16
	db        0,1,2,3,0,48,0,0,0,161,16,0,0,161,16,0
	db        0,26,0,9,0,139,16,0,0,135,16,0,0,140,16,0
	db        0,4,0,1,0,164,16,0,0,0,0,0,0,8,0,1
	db        2,1,0,253,255,255,255,8,0,1,0,1,0,16,0,0
	db        0,8,0,1,0,1,0,33,0,0,0,8,0,1,0,1
	db        0,249,255,255,255,8,0,2,0,10,0,169,16,0,0,14
	db        0,8,0,33,0,0,0,7,0,1,0,170,16,0,0,8
	db        0,1,2,1,0,34,0,0,0,8,0,2,0,10,0,172
	db        16,0,0,14,0,8,0,33,0,0,0,7,0,2,0,173
	db        16,0,0,12,0,1,2,2,0,34,0,0,0,32,0,0
	db        0,8,0,2,0,10,0,175,16,0,0,14,0,8,0,33
	db        0,0,0,7,0,2,0,176,16,0,0,12,0,1,2,2
	db        0,34,0,0,0,33,0,0,0,8,0,1,0,1,0,116
	db        0,0,0,14,0,8,0,3,0,0,0,0,0,1,0,179
	db        16,0,0,8,0,1,2,1,0,3,4,0,0,8,0,2
	db        0,10,2,3,0,0,0,14,0,8,0,3,0,0,0,4
	db        0,1,0,182,16,0,0,8,0,1,2,1,0,3,4,0
	db        0,14,0,8,0,3,0,0,0,7,0,1,0,184,16,0
	db        0,8,0,1,2,1,0,34,0,0,0,14,0,8,0,3
	db        4,0,0,0,0,1,0,186,16,0,0,8,0,1,2,1
	db        0,117,0,0,0,14,0,8,0,3,0,0,0,0,0,2
	db        0,188,16,0,0,12,0,1,2,2,0,116,0,0,0,116
	db        0,0,0,14,0,8,0,3,0,0,0,4,0,2,0,191
	db        16,0,0,8,0,2,0,10,4,248,255,255,255,12,0,1
	db        2,2,0,190,16,0,0,116,0,0,0,14,0,8,0,18
	db        0,0,0,4,0,3,0,193,16,0,0,16,0,1,2,3
	db        0,160,16,0,0,252,255,255,255,3,4,0,0,14,0,8
	db        0,3,0,0,0,4,0,1,0,196,16,0,0,8,0,2
	db        0,10,4,247,255,255,255,8,0,1,2,1,0,195,16,0
	db        0,14,0,8,0,3,0,0,0,4,0,1,0,198,16,0
	db        0,8,0,1,2,1,0,160,16,0,0,14,0,8,0,3
	db        0,0,0,4,0,2,0,200,16,0,0,12,0,1,2,2
	db        0,195,16,0,0,3,4,0,0,14,0,8,0,3,0,0
	db        0,4,0,2,0,202,16,0,0,12,0,1,2,2,0,195
	db        16,0,0,3,4,0,0,14,0,8,0,252,255,255,255,4
	db        0,2,0,204,16,0,0,12,0,1,2,2,0,195,16,0
	db        0,246,255,255,255,14,0,8,0,3,0,0,0,4,0,1
	db        0,206,16,0,0,8,0,1,2,1,0,195,16,0,0,14
	db        0,8,0,3,0,0,0,4,0,2,0,209,16,0,0,8
	db        0,2,0,10,4,245,255,255,255,12,0,1,2,2,0,208
	db        16,0,0,244,255,255,255,14,0,8,0,3,0,0,0,4
	db        0,2,0,211,16,0,0,12,0,1,2,2,0,160,16,0
	db        0,243,255,255,255,14,0,8,0,3,0,0,0,4,0,2
	db        0,213,16,0,0,12,0,1,2,2,0,208,16,0,0,244
	db        255,255,255,14,0,8,0,3,0,0,0,4,0,1,0,215
	db        16,0,0,8,0,1,2,1,0,160,16,0,0,14,0,8
	db        0,3,0,0,0,4,0,3,0,217,16,0,0,16,0,1
	db        2,3,0,160,16,0,0,253,255,255,255,242,255,255,255,14
	db        0,8,0,3,0,0,0,4,0,2,0,219,16,0,0,12
	db        0,1,2,2,0,160,16,0,0,241,255,255,255,14,0,8
	db        0,3,0,0,0,4,0,2,0,221,16,0,0,12,0,1
	db        2,2,0,160,16,0,0,161,16,0,0,14,0,8,0,3
	db        0,0,0,4,0,4,0,223,16,0,0,20,0,1,2,4
	db        0,160,16,0,0,253,255,255,255,161,16,0,0,161,16,0
	db        0,14,0,8,0,3,0,0,0,4,0,2,0,225,16,0
	db        0,12,0,1,2,2,0,160,16,0,0,240,255,255,255,14
	db        0,8,0,18,0,0,0,7,0,3,0,227,16,0,0,16
	db        0,1,2,3,0,160,16,0,0,239,255,255,255,3,4,0
	db        0,14,0,8,0,3,0,0,0,4,0,2,0,230,16,0
	db        0,8,0,2,0,10,4,254,255,255,255,12,0,1,2,2
	db        0,229,16,0,0,16,0,0,0,14,0,8,0,116,0,0
	db        0,2,0,0,0,232,16,0,0,4,0,1,2,0,0,14
	db        0,8,0,238,255,255,255,4,0,2,0,235,16,0,0,8
	db        0,2,0,10,4,250,255,255,255,12,0,1,2,2,0,234
	db        16,0,0,237,255,255,255,14,0,8,0,3,0,0,0,4
	db        0,4,0,237,16,0,0,20,0,1,2,4,0,246,255,255
	db        255,48,0,0,0,161,16,0,0,161,16,0,0,14,0,8
	db        0,3,0,0,0,4,0,2,0,239,16,0,0,12,0,1
	db        2,2,0,234,16,0,0,116,0,0,0,14,0,8,0,236
	db        255,255,255,4,0,3,0,241,16,0,0,16,0,1,2,3
	db        0,229,16,0,0,16,0,0,0,253,255,255,255,14,0,8
	db        0,3,0,0,0,4,0,2,0,246,16,0,0,14,0,52
	db        0,3,0,0,0,4,0,2,0,245,16,0,0,8,0,2
	db        0,10,4,3,0,0,0,12,0,1,2,2,0,244,16,0
	db        0,252,255,255,255,12,0,1,2,2,0,229,16,0,0,243
	db        16,0,0,14,0,8,0,3,0,0,0,4,0,2,0,248
	db        16,0,0,12,0,1,2,2,0,160,16,0,0,16,0,0
	db        0,14,0,8,0,3,0,0,0,4,0,2,0,250,16,0
	db        0,12,0,1,2,2,0,208,16,0,0,16,0,0,0,14
	db        0,8,0,3,0,0,0,4,0,2,0,252,16,0,0,12
	db        0,1,2,2,0,195,16,0,0,16,0,0,0
$$BTYPES	ends
$$BNAMES	segment byte public use32 'DEBNAM'
	db	8,'outportb'
	db	5,'wPort'
	db	4,'data'
	db	7,'outport'
	db	5,'wPort'
	db	4,'data'
	db	6,'inport'
	db	5,'wPort'
	db	7,'program'
	db	8,'ChipData'
	db	10,'DataBuffer'
	db	6,'Length'
	db	5,'Error'
	db	19,'@ChipData@$bctr$qpc'
	db	18,'@ChipData@$bdtr$qv'
	db	13,'GetDataBuffer'
	db	9,'GetLength'
	db	8,'GetError'
	db	15,'ChipDataObjects'
	db	9,'ArrayMask'
	db	14,'ArrayChipsPort'
	db	12,'ProgChipPort'
	db	14,'InitializeProg'
	db	15,'InitializeArray'
	db	12,'ProgramArray'
	db	17,'CreateProgramWord'
	db	17,'CreateProgramMask'
	db	13,'ConfigureProg'
	db	10,'DoProgProg'
	db	11,'DoProgVideo'
	db	10,'DoProgAce2'
	db	17,'@program@$bctr$qv'
	db	17,'@program@$bdtr$qv'
	db	8,'SetPorts'
	db	11,'DoConfigure'
	db	9,'SetFormat'
	db	5,'delay'
	db	11,'X2InitChips'
	db	6,'ProgX2'
	db	8,'ReProgX2'
	db	17,'@program@$bdtr$qv'
	db	4,'this'
	db	1,'i'
	db	17,'@program@SetPorts'
	db	4,'this'
	db	5,'PPort'
	db	5,'APort'
	db	14,'@program@delay'
	db	4,'this'
	db	4,'loop'
	db	1,'j'
	db	18,'@program@SetFormat'
	db	4,'this'
	db	7,'ChipNum'
	db	8,'FileName'
	db	9,'_ChipData'
	db	23,'@program@InitializeProg'
	db	4,'this'
	db	24,'@program@InitializeArray'
	db	4,'this'
	db	8,'WordMask'
	db	9,'wBasePort'
	db	11,'WorkingMask'
	db	9,'init_port'
	db	21,'@program@ProgramArray'
	db	4,'this'
	db	10,'DataLength'
	db	2,'pp'
	db	5,'nextL'
	db	2,'dL'
	db	26,'@program@CreateProgramWord'
	db	4,'this'
	db	5,'PWord'
	db	1,'i'
	db	10,'DataBuffer'
	db	2,'tw'
	db	3,'tmp'
	db	26,'@program@CreateProgramMask'
	db	4,'this'
	db	10,'longLength'
	db	1,'i'
	db	22,'@program@ConfigureProg'
	db	4,'this'
	db	1,' '
	db	20,'@program@DoConfigure'
	db	4,'this'
	db	5,'WType'
	db	20,'@program@DoProgVideo'
	db	4,'this'
	db	9,'wBasePort'
	db	6,'start1'
	db	6,'length'
	db	13,'lastPossition'
	db	5,'wPort'
	db	1,'l'
	db	1,'c'
	db	19,'@program@DoProgAce2'
	db	4,'this'
	db	9,'wBasePort'
	db	6,'start1'
	db	6,'start2'
	db	6,'length'
	db	13,'lastPossition'
	db	5,'wPort'
	db	1,'l'
	db	1,'d'
	db	1,'c'
	db	19,'@program@DoProgProg'
	db	4,'this'
	db	9,'wBasePort'
	db	6,'start1'
	db	6,'length'
	db	2,'pp'
	db	5,'nextL'
	db	2,'dL'
	db	5,'wPort'
	db	1,'l'
	db	1,'c'
	db	20,'@program@X2InitChips'
	db	4,'this'
	db	9,'wBasePort'
	db	9,'init_port'
	db	15,'@program@ProgX2'
	db	4,'this'
	db	9,'wBasePort'
	db	6,'start1'
	db	12,'ProgressBar2'
	db	6,'length'
	db	13,'lastPossition'
	db	5,'wPort'
	db	1,'l'
	db	1,'c'
	db	17,'@program@ReProgX2'
	db	4,'this'
	db	9,'wBasePort'
	db	6,'start1'
	db	12,'ProgressBar2'
	db	6,'length'
	db	5,'wPort'
	db	1,'l'
	db	1,'c'
	db	5,'IRand'
	db	5,'IRand'
	db	1,'N'
	db	5,'irand'
	db	11,'RandomStart'
	db	18,'@ChipData@GetError'
	db	4,'this'
	db	10,'TVivaTimer'
	db	42,'@TVivaTimer@$bctr$qqrp18Classes@TComponent'
	db	10,'TimerTimer'
	db	32,'@TVivaTimer@$basg$qr10TVivaTimer'
	db	22,'@TVivaTimer@$bdtr$qqrv'
	db	22,'@TVivaTimer@$bdtr$qqrv'
	db	4,'this'
	db	34,'@Classes@TComponent@UpdateRegistry'
	db	4,'this'
	db	8,'Register'
	db	7,'ClassID'
	db	6,'ProgID'
	db	42,'@TVivaTimer@$bctr$qqrp18Classes@TComponent'
	db	4,'this'
	db	6,'AOwner'
	db	9,'hDlPortIO'
	db	18,'lpDlReadPortUshort'
	db	18,'lpDlWritePortUchar'
	db	19,'lpDlWritePortUshort'
	db	18,'CurrentBytePointer'
	db	17,'CurrentBitPointer'
	db	5,'dseed'
	db	3,'std'
	db	6,'System'
	db	19,'@System@varSmallint'
	db	18,'@System@varInteger'
	db	17,'@System@varSingle'
	db	17,'@System@varDouble'
	db	19,'@System@varCurrency'
	db	15,'@System@varDate'
	db	17,'@System@varOleStr'
	db	19,'@System@varDispatch'
	db	18,'@System@varBoolean'
	db	18,'@System@varVariant'
	db	18,'@System@varUnknown'
	db	15,'@System@varByte'
	db	16,'@System@varByRef'
	db	17,'@System@vtInteger'
	db	17,'@System@vtBoolean'
	db	14,'@System@vtChar'
	db	18,'@System@vtExtended'
	db	16,'@System@vtString'
	db	17,'@System@vtPointer'
	db	15,'@System@vtPChar'
	db	16,'@System@vtObject'
	db	15,'@System@vtClass'
	db	18,'@System@vtWideChar'
	db	19,'@System@vtPWideChar'
	db	20,'@System@vtAnsiString'
	db	18,'@System@vtCurrency'
	db	17,'@System@vtVariant'
	db	8,'Sysutils'
	db	7,'Sysinit'
	db	7,'Windows'
	db	8,'Messages'
	db	8,'Sysconst'
	db	7,'Classes'
	db	8,'Graphics'
	db	17,'@Graphics@clBlack'
	db	8,'Commctrl'
	db	7,'Imglist'
	db	8,'Actnlist'
	db	3,'Imm'
	db	7,'Contnrs'
	db	5,'Menus'
	db	8,'Multimon'
	db	8,'Controls'
	db	5,'Forms'
	db	7,'Activex'
	db	6,'Urlmon'
	db	7,'Wininet'
	db	6,'Regstr'
	db	8,'Shellapi'
	db	6,'Shlobj'
	db	8,'Stdctrls'
	db	7,'Commdlg'
	db	7,'Dialogs'
	db	8,'Extctrls'
	db	2,'Db'
	db	7,'Toolwin'
	db	8,'Richedit'
	db	8,'Comctrls'
	db	7,'Buttons'
	db	4,'Mask'
	db	7,'Dbctrls'
	db	6,'Smintf'
	db	3,'Bde'
	db	8,'Dbcommon'
	db	8,'Dbtables'
	db	6,'Stdvcl'
	db	6,'Comobj'
	db	7,'Axctrls'
	db	8,'Olectrls'
	db	4,'Ole2'
	db	6,'Olectl'
	db	7,'Chartfx'
	db	8,'Dbcgrids'
	db	5,'Grids'
	db	7,'Dbgrids'
	db	8,'Dblookup'
	db	5,'Ddeml'
	db	6,'Ddeman'
	db	8,'Filectrl'
	db	8,'Graphsvr'
	db	8,'Mmsystem'
	db	7,'Mplayer'
	db	6,'Oledlg'
	db	8,'Olectnrs'
	db	7,'Outline'
	db	8,'Qr3const'
	db	6,'Qrexpr'
	db	8,'Qrexpbld'
	db	8,'Winspool'
	db	8,'Printers'
	db	7,'Qrprntr'
	db	8,'Qrprgres'
	db	8,'Quickrpt'
	db	8,'Tabnotbk'
	db	4,'Tabs'
	db	8,'Vcfimprs'
	db	8,'Vcfrmla1'
	db	8,'Vcspell3'
	db	9,'VivaUtils'
	db	18,'LPDLREADPORTUSHORT'
	db	18,'LPDLWRITEPORTUCHAR'
	db	19,'LPDLWRITEPORTUSHORT'
	db	8,'ChipData'
	db	10,'TVivaTimer'
	db	11,'VivaGlobals'
	db	27,'@VivaGlobals@SGDataSetCount'
	db	7,'program'
	db	11,'@std@time_t'
	db	12,'@std@clock_t'
	db	11,'VivaGlobals'
	db	9,'VivaUtils'
	db	8,'Vcspell3'
	db	8,'Vcfrmla1'
	db	8,'Vcfimprs'
	db	4,'Tabs'
	db	8,'Tabnotbk'
	db	8,'Quickrpt'
	db	8,'Qrprgres'
	db	7,'Qrprntr'
	db	8,'Printers'
	db	8,'Winspool'
	db	8,'Qrexpbld'
	db	6,'Qrexpr'
	db	8,'Qr3const'
	db	7,'Outline'
	db	8,'Olectnrs'
	db	6,'Oledlg'
	db	7,'Mplayer'
	db	8,'Mmsystem'
	db	8,'Graphsvr'
	db	8,'Filectrl'
	db	6,'Ddeman'
	db	5,'Ddeml'
	db	8,'Dblookup'
	db	7,'Dbgrids'
	db	5,'Grids'
	db	8,'Dbcgrids'
	db	7,'Chartfx'
	db	6,'Olectl'
	db	4,'Ole2'
	db	8,'Olectrls'
	db	7,'Axctrls'
	db	6,'Comobj'
	db	6,'Stdvcl'
	db	8,'Dbtables'
	db	8,'Dbcommon'
	db	3,'Bde'
	db	6,'Smintf'
	db	7,'Dbctrls'
	db	4,'Mask'
	db	7,'Buttons'
	db	8,'Comctrls'
	db	8,'Richedit'
	db	7,'Toolwin'
	db	2,'Db'
	db	8,'Extctrls'
	db	7,'Dialogs'
	db	7,'Commdlg'
	db	8,'Stdctrls'
	db	6,'Shlobj'
	db	8,'Shellapi'
	db	6,'Regstr'
	db	7,'Wininet'
	db	6,'Urlmon'
	db	7,'Activex'
	db	5,'Forms'
	db	8,'Controls'
	db	8,'Multimon'
	db	5,'Menus'
	db	7,'Contnrs'
	db	3,'Imm'
	db	8,'Actnlist'
	db	7,'Imglist'
	db	8,'Commctrl'
	db	8,'Graphics'
	db	7,'Classes'
	db	8,'Sysutils'
	db	8,'Sysconst'
	db	8,'Messages'
	db	7,'Windows'
	db	7,'Sysinit'
	db	6,'System'
	db	11,'_setsystime'
	db	11,'_getsystime'
	db	5,'tzset'
	db	8,'_strtime'
	db	8,'_strdate'
	db	6,'_tzset'
	db	5,'stime'
	db	7,'_wtzset'
	db	8,'wcsftime'
	db	9,'_wstrtime'
	db	9,'_wstrdate'
	db	7,'_wctime'
	db	9,'_wasctime'
	db	9,'_lstrftim'
	db	8,'strftime'
	db	5,'clock'
	db	6,'mktime'
	db	9,'localtime'
	db	6,'gmtime'
	db	8,'difftime'
	db	5,'ctime'
	db	7,'asctime'
	db	6,'pow10l'
	db	5,'polyl'
	db	6,'hypotl'
	db	4,'poly'
	db	5,'hypot'
	db	5,'pow10'
	db	4,'cosl'
	db	4,'sinl'
	db	3,'cos'
	db	3,'sin'
	db	4,'tanl'
	db	5,'tanhl'
	db	5,'sqrtl'
	db	5,'sinhl'
	db	4,'powl'
	db	5,'modfl'
	db	4,'logl'
	db	6,'log10l'
	db	6,'ldexpl'
	db	6,'frexpl'
	db	5,'fmodl'
	db	6,'floorl'
	db	5,'fabsl'
	db	4,'expl'
	db	5,'coshl'
	db	5,'ceill'
	db	5,'atanl'
	db	6,'atan2l'
	db	5,'asinl'
	db	5,'acosl'
	db	4,'tanh'
	db	3,'tan'
	db	4,'sqrt'
	db	4,'sinh'
	db	3,'pow'
	db	4,'modf'
	db	5,'log10'
	db	3,'log'
	db	5,'ldexp'
	db	5,'frexp'
	db	4,'fmod'
	db	5,'floor'
	db	4,'fabs'
	db	3,'exp'
	db	4,'cosh'
	db	4,'ceil'
	db	5,'atan2'
	db	4,'atan'
	db	4,'asin'
	db	4,'acos'
	db	8,'heapwalk'
	db	13,'heapchecknode'
	db	13,'heapcheckfree'
	db	12,'heapfillfree'
	db	9,'heapcheck'
	db	11,'_wgetcurdir'
	db	8,'_wgetcwd'
	db	9,'_wfnmerge'
	db	9,'_wfnsplit'
	db	11,'_wfindclose'
	db	10,'_wfindnext'
	db	11,'_wfindfirst'
	db	8,'_wmktemp'
	db	11,'wsearchpath'
	db	7,'_wrmdir'
	db	7,'_wmkdir'
	db	7,'_wchdir'
	db	5,'rmdir'
	db	6,'mktemp'
	db	5,'mkdir'
	db	7,'setdisk'
	db	10,'searchpath'
	db	6,'_rmdir'
	db	7,'_mktemp'
	db	6,'_mkdir'
	db	7,'getdisk'
	db	6,'getcwd'
	db	9,'getcurdir'
	db	7,'fnsplit'
	db	7,'fnmerge'
	db	9,'findclose'
	db	8,'findnext'
	db	9,'findfirst'
	db	5,'chdir'
	db	5,'_rotr'
	db	5,'_rotl'
	db	6,'_crotr'
	db	6,'_lrotr'
	db	6,'_lrotl'
	db	6,'_crotl'
	db	3,'abs'
	db	6,'wctomb'
	db	7,'wcstoul'
	db	8,'wcstombs'
	db	6,'wcstol'
	db	6,'wcstod'
	db	5,'ultoa'
	db	4,'time'
	db	6,'system'
	db	4,'swab'
	db	7,'strtoul'
	db	6,'strtol'
	db	6,'strtod'
	db	5,'srand'
	db	7,'realloc'
	db	4,'rand'
	db	6,'putenv'
	db	6,'perror'
	db	6,'mbtowc'
	db	8,'mbstowcs'
	db	5,'mblen'
	db	13,'_seterrormode'
	db	26,'@std@%max$T1%$qrxT1t1$rxT1'
	db	26,'@std@%min$T1%$qrxT1t1$rxT1'
	db	17,'@std@randomize$qv'
	db	14,'@std@random$qi'
	db	4,'ltoa'
	db	4,'gcvt'
	db	4,'fcvt'
	db	4,'ecvt'
	db	6,'malloc'
	db	4,'ldiv'
	db	4,'labs'
	db	4,'itoa'
	db	6,'getenv'
	db	4,'free'
	db	4,'exit'
	db	3,'div'
	db	6,'calloc'
	db	4,'atol'
	db	4,'atoi'
	db	4,'atof'
	db	6,'atexit'
	db	5,'abort'
	db	6,'_wtold'
	db	5,'_wtol'
	db	5,'_wtoi'
	db	5,'_wtof'
	db	8,'_wsystem'
	db	11,'_wsplitpath'
	db	11,'_wsearchstr'
	db	11,'_wsearchenv'
	db	8,'_wputenv'
	db	8,'_wperror'
	db	10,'_wmakepath'
	db	8,'_wgetenv'
	db	10,'_wfullpath'
	db	8,'_wcstold'
	db	8,'_strtold'
	db	10,'_splitpath'
	db	10,'_searchstr'
	db	10,'_searchenv'
	db	9,'_makepath'
	db	7,'_wtoi64'
	db	8,'_ui64tow'
	db	8,'_ui64toa'
	db	7,'_i64tow'
	db	7,'_i64toa'
	db	7,'_atoi64'
	db	5,'_ltow'
	db	5,'_ltoa'
	db	6,'_lrand'
	db	5,'_itow'
	db	6,'_ultow'
	db	5,'_gcvt'
	db	5,'_fcvt'
	db	9,'_fullpath'
	db	5,'_exit'
	db	6,'_atold'
	db	5,'_ecvt'
	db	7,'__errno'
	db	10,'__doserrno'
	db	5,'qsort'
	db	7,'lsearch'
	db	5,'lfind'
	db	7,'bsearch'
	db	7,'wcsxfrm'
	db	6,'wcstok'
	db	6,'wcsstr'
	db	6,'wcsspn'
	db	7,'wcsrchr'
	db	7,'wcspcpy'
	db	7,'wcspbrk'
	db	7,'wcsncpy'
	db	7,'wcsncmp'
	db	7,'wcsncat'
	db	6,'wcslen'
	db	7,'wcscspn'
	db	6,'wcscpy'
	db	7,'wcscoll'
	db	6,'wcscmp'
	db	6,'wcschr'
	db	6,'wcscat'
	db	6,'strtok'
	db	18,'@std@strstr$qpxct1'
	db	18,'@std@strstr$qpcpxc'
	db	6,'strspn'
	db	19,'@std@strpbrk$qpxct1'
	db	19,'@std@strpbrk$qpcpxc'
	db	8,'strerror'
	db	7,'strcspn'
	db	18,'@std@strrchr$qpxci'
	db	17,'@std@strrchr$qpci'
	db	7,'strncpy'
	db	7,'strncmp'
	db	7,'strncat'
	db	6,'strlen'
	db	6,'strcpy'
	db	6,'strcmp'
	db	17,'@std@strchr$qpxci'
	db	16,'@std@strchr$qpci'
	db	6,'strcat'
	db	6,'strset'
	db	6,'stpcpy'
	db	7,'strnset'
	db	7,'_wcsset'
	db	7,'_wcsrev'
	db	8,'_wcspcpy'
	db	8,'_wcsnset'
	db	10,'_wcsnicoll'
	db	9,'_wcsnicmp'
	db	9,'_wcsncoll'
	db	9,'_wcsicoll'
	db	8,'_wcsicmp'
	db	7,'_wcsdup'
	db	10,'_strnicoll'
	db	9,'_strncoll'
	db	9,'_stricoll'
	db	9,'_strerror'
	db	9,'_lwcsxfrm'
	db	8,'_lwcsupr'
	db	11,'_lwcsnicoll'
	db	10,'_lwcsncoll'
	db	8,'_lwcslwr'
	db	10,'_lwcsicoll'
	db	9,'_lwcscoll'
	db	9,'_lstrxfrm'
	db	11,'_lstrnicoll'
	db	10,'_lstrncoll'
	db	10,'_lstricoll'
	db	9,'_lstrcoll'
	db	6,'strrev'
	db	8,'strnicmp'
	db	7,'stricmp'
	db	6,'strdup'
	db	7,'_stpcpy'
	db	8,'_lstrupr'
	db	8,'_lstrlwr'
	db	12,'_lwsetlocale'
	db	11,'_wsetlocale'
	db	12,'_llocaleconv'
	db	10,'localeconv'
	db	11,'_lsetlocale'
	db	10,'_ltowlower'
	db	10,'_ltowupper'
	db	8,'towupper'
	db	8,'towlower'
	db	9,'_ltoupper'
	db	7,'toupper'
	db	9,'_ltolower'
	db	7,'tolower'
	db	8,'iswascii'
	db	9,'iswxdigit'
	db	8,'iswupper'
	db	8,'iswspace'
	db	8,'iswpunct'
	db	8,'iswprint'
	db	8,'iswlower'
	db	8,'iswgraph'
	db	8,'iswdigit'
	db	8,'iswcntrl'
	db	8,'iswalpha'
	db	8,'iswalnum'
	db	7,'isascii'
	db	8,'isxdigit'
	db	7,'isupper'
	db	7,'isspace'
	db	7,'ispunct'
	db	7,'isprint'
	db	7,'islower'
	db	7,'isgraph'
	db	7,'isdigit'
	db	7,'iscntrl'
	db	7,'isalpha'
	db	7,'isalnum'
	db	6,'memset'
	db	6,'memcpy'
	db	6,'memcmp'
	db	19,'@std@memchr$qpxviui'
	db	18,'@std@memchr$qpviui'
	db	7,'memmove'
	db	7,'memicmp'
	db	7,'memccpy'
	db	8,'_wmemset'
	db	8,'_wmemcpy'
	db	21,'@std@_wmemchr$qpxviui'
	db	20,'@std@_wmemchr$qpviui'
	db	10,'__threadid'
	db	14,'@std@ptrdiff_t'
	db	11,'@std@size_t'
	db	11,'@std@wint_t'
	db	13,'@std@wctype_t'
	db	12,'@std@va_list'
	db	9,'_chartype'
	db	6,'_lower'
	db	6,'_upper'
	db	5,'lconv'
	db	5,'_argc'
	db	5,'_argv'
	db	12,'__mb_cur_max'
	db	16,'_cmdline_escapes'
	db	8,'_environ'
	db	9,'_fileinfo'
	db	6,'_fmode'
	db	8,'_osmajor'
	db	8,'_osminor'
	db	7,'_osmode'
	db	10,'_osversion'
	db	12,'_sys_errlist'
	db	9,'_sys_nerr'
	db	9,'_wenviron'
	db	13,'@std@atexit_t'
	db	10,'@std@div_t'
	db	11,'@std@ldiv_t'
	db	6,'_TCHAR'
	db	7,'_TSCHAR'
	db	7,'_TUCHAR'
	db	7,'_TXCHAR'
	db	5,'_TINT'
	db	10,'@std@ffblk'
	db	12,'@std@_wffblk'
	db	13,'@std@heapinfo'
	db	10,'_huge_dble'
	db	11,'_huge_ldble'
	db	11,'@std@time_t'
	db	12,'@std@clock_t'
	db	7,'@std@tm'
	db	9,'_daylight'
	db	9,'_timezone'
	db	7,'_tzname'
	db	8,'_wtzname'
$$BNAMES	ends
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\time.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\math.h" 10303 10240
	?debug	D "F:\DAIODeliverable\VivaSW\MessageInfo.h" 14481 32010
	?debug	D "F:\DAIODeliverable\VivaSW\StringList.h" 14481 32011
	?debug	D "F:\DAIODeliverable\VivaSW\Errortrap.h" 14481 32010
	?debug	D "F:\DAIODeliverable\VivaSW\ChipData.h" 14481 32009
	?debug	D "F:\DAIODeliverable\VivaSW\Asm.h" 14481 32009
	?debug	D "F:\DAIODeliverable\VivaSW\Utils.h" 14481 32011
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\vcl4.h" 10303 10240
	?debug	D "F:\DAIODeliverable\VivaSW\Globals.h" 14481 32506
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\databkr.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\stddef.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\alloc.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\malloc.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\dir.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\VCSpell3.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\VCFrmla1.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\VCFImprs.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Tabs.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Tabnotbk.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\WinSpool.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Printers.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\QRPrntr.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\QRPrgres.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\QRExpr.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\qr3const.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\QRExpbld.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Quickrpt.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Outline.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\DocObj.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\dlgs.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\tchar.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\oledlg.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\OleDlg.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\OleCtnrs.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\mmsystem.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\MMSystem.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\MPlayer.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\GraphSvr.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\FileCtrl.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\ddeml.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Ddeml.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\DdeMan.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\DBLookup.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Grids.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\DBGrids.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\DBCGrids.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\olectl.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\propidl.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\cguid.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\objbase.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\ole2.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\OLE2.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\OleCtl.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\msxml.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\urlmon.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\servprov.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\oleidl.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\ocidl.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\objsafe.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\ComObj.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\StdVCL.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\AxCtrls.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\OleCtrls.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\ChartFX.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\DBCommon.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Bde.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\SMIntf.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\dbtables.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Mask.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Buttons.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\RichEdit.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\RichEdit.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\ToolWin.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\ComCtrls.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Db.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\dbctrls.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\extctrls.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\commdlg.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\CommDlg.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\StdCtrls.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\shellapi.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\ShellAPI.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\regstr.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\RegStr.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\wininet.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\WinInet.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\objidl.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\oaidl.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\oleauto.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\ActiveX.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\UrlMon.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\ShlObj.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\dialogs.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\forms.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\multimon.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\MultiMon.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Contnrs.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Menus.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Imm.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\CommCtrl.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\Commctrl.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\ImgList.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\ActnList.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\controls.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\graphics.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\classes.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\SysConst.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\sysutils.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\messages.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\dde.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\wincrypt.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\wintrust.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\SysInit.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\windows.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\unknwn.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\sysvari.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\sysopen.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\search.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\stdlib.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\sysdyn.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\systvar.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\sysset.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\syscomp.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\syscurr.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\systdate.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\systobj.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\rpcnsip.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\rpcndr.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\rpcnterr.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\rpcnsi.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\rpcdcep.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\rpcdce.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\rpc.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\wtypes.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\wstring.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\sysmac.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\dstring.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\sysclass.H" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\basetyps.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\prsht.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\winspool.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\imm.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\mcx.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\winsvc.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\winnetwk.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\winreg.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\winver.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\wincon.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\winnls.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\tvout.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\winuser.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\pshpack1.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\wingdi.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\winerror.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\winbase.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\pshpack8.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\pshpack2.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\poppack.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\pshpack4.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\_loc.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\locale.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\_str.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\string.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\guiddef.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\basetsd.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\mbctype.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\ctype.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\winnt.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\windef.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\stdarg.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\excpt.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\_null.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\_defs.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\_stddef.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\mem.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\windows.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\sysmac.H" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\system.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\vcl0.h" 10303 10240
	?debug	D "C:\PROGRAM FILES (X86)\BORLAND\CBUILDER5\INCLUDE\VCL\VCL4.H" 10303 10240
	?debug	D "F:\DAIODeliverable\VivaSW\Asm.cpp" 14481 32009
 ?debug  C FB0608566976612E233030E33B34004559BC4E
 ?debug  C FB050E232A5450726F6772657373426172
 ?debug  C FB0507235454696D6572
 ?debug  C FB050C232A54436F6D706F6E656E74
 ?debug  C FB0509232A544F626A656374
 ?debug  C FB050B2354436F6D706F6E656E74
 ?debug  C FB050B23416E7369537472696E67
 ?debug  C FB05072354436F6C6F72
 ?debug  C FB050D235450726F6772657373426172
 ?debug  C FB050823544F626A656374
 ?debug  C FB050C232A544D657461436C617373
 ?debug  C FB050C235450657273697374656E74
 ?debug  C FB050D232A5450657273697374656E74
 ?debug  C FB0508232A5446696C6572
 ?debug  C FB050B23544F7065726174696F6E
 ?debug  C FB0509232A54526561646572
 ?debug  C FB0509232A54577269746572
 ?debug  C FB050723265F47554944
 ?debug  C FB050C232A416E7369537472696E67
 ?debug  C FB050C2326416E7369537472696E67
 ?debug  C FB0508232A5454696D6572
	end
