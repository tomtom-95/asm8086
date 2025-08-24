not ah
not bl
not sp
not si
not word [bp]
not byte [bp + 9905]

shl ah, 1
shr ax, 1
sar bx, 1
rol cx, 1
ror dh, 1
rcl sp, 1
rcr bp, 1

shl word [bp + 5], 1
shr byte [bx + si - 199], 1
sar byte [bx + di - 300], 1
rol word [bp], 1
ror word [4938], 1
rcl byte [3], 1
rcr word [bx], 1

shl ah, cl
shr ax, cl
sar bx, cl
rol cx, cl
ror dh, cl
rcl sp, cl
rcr bp, cl

shl word [bp + 5], cl
shr word [bx + si - 199], cl
sar byte [bx + di - 300], cl
rol byte [bp], cl
ror byte [4938], cl
rcl byte [3], cl
rcr word [bx], cl