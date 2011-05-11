.model small
.stack 1024
.data
newline	db	10,13,'$'
x	dw	?
y	dw	?
tep	dw	?
strvar	db	?,'$'
.code
.startup

;INStmt
CALL INPUT
MOV y,AX
;EndINStmt

;AssignmentStmt
;Expression
;Expression
;Expression
MOV AX,y
;EndExpression
PUSH AX
;Expression
;Expression
MOV AX,45
;EndExpression
PUSH AX
;Expression
MOV AX,2
;EndExpression
MOV BX,AX
POP AX
PUSH DX
MUL BX
POP DX
;EndExpression
MOV BX,AX
POP AX
SUB AX,BX
;EndExpression
PUSH AX
;Expression
;Expression
;Expression
MOV AX,y
;EndExpression
PUSH AX
;Expression
MOV AX,5
;EndExpression
MOV BX,AX
POP AX
PUSH DX
MOV DX,0
DIV BX
POP DX
;EndExpression
PUSH AX
;Expression
MOV AX,10
;EndExpression
MOV BX,AX
POP AX
PUSH DX
MUL BX
POP DX
;EndExpression
MOV BX,AX
POP AX
ADD AX,BX
;EndExpression
MOV y,AX
;EndAssignmentStmt

;OUTStmt
MOV AX,y
CALL OUTPUT
;EndOUTStmt

;AssignmentStmt
;Expression
;Expression
;Expression
MOV AX,3
;EndExpression
PUSH AX
;Expression
MOV AX,2
;EndExpression
MOV BX,AX
POP AX
SUB AX,BX
;EndExpression
PUSH AX
;Expression
MOV AX,1
;EndExpression
MOV BX,AX
POP AX
SUB AX,BX
;EndExpression
MOV x,AX
;EndAssignmentStmt

;OUTStmt
MOV AX,x
CALL OUTPUT
;EndOUTStmt

;INStmt
CALL INPUT
MOV x,AX
;EndINStmt

;ConditionalStmt
MOV AX,x
MOV BX,y
CMP AX,BX
;BoolExpression
JNG Label0
;EndBoolExpression

;AssignmentStmt
;Expression
;Expression
MOV AX,y
;EndExpression
PUSH AX
;Expression
MOV AX,2
;EndExpression
MOV BX,AX
POP AX
PUSH DX
MUL BX
POP DX
;EndExpression
MOV y,AX
;EndAssignmentStmt

;ConditionalStmt
MOV AX,x
MOV BX,y
CMP AX,BX
;BoolExpression
JNG Label1
;EndBoolExpression

;AssignmentStmt
;Expression
;Expression
;Expression
MOV AX,3
;EndExpression
PUSH AX
;Expression
MOV AX,2
;EndExpression
MOV BX,AX
POP AX
SUB AX,BX
;EndExpression
PUSH AX
;Expression
MOV AX,1
;EndExpression
MOV BX,AX
POP AX
SUB AX,BX
;EndExpression
MOV x,AX
;EndAssignmentStmt

;OUTStmt
MOV AX,x
CALL OUTPUT
;EndOUTStmt
;EndConditionalStmt
Label1:

;AssignmentStmt
;Expression
;Expression
MOV AX,3
;EndExpression
PUSH AX
;Expression
;Expression
MOV AX,2
;EndExpression
PUSH AX
;Expression
MOV AX,1
;EndExpression
MOV BX,AX
POP AX
SUB AX,BX
;EndExpression
MOV BX,AX
POP AX
SUB AX,BX
;EndExpression
MOV x,AX
;EndAssignmentStmt

;OUTStmt
MOV AX,x
CALL OUTPUT
;EndOUTStmt
;EndConditionalStmt
Label0:

;AssignmentStmt
;Expression
;Expression
MOV AX,y
;EndExpression
PUSH AX
;Expression
MOV AX,2
;EndExpression
MOV BX,AX
POP AX
PUSH DX
MOV DX,0
DIV BX
POP DX
;EndExpression
MOV y,AX
;EndAssignmentStmt

;AssignmentStmt
;Expression
MOV AX,200
;EndExpression
MOV tep,AX
;EndAssignmentStmt

;RepititiveStmt
Label2:
MOV AX,y
MOV BX,tep
CMP AX,BX
;BoolExpression
JNLE Label3
;EndBoolExpression

;AssignmentStmt
;Expression
;Expression
MOV AX,y
;EndExpression
PUSH AX
;Expression
MOV AX,1
;EndExpression
MOV BX,AX
POP AX
ADD AX,BX
;EndExpression
MOV y,AX
;EndAssignmentStmt

;OUTStmt
MOV AX,y
CALL OUTPUT
;EndOUTStmt
JMP Label2
;EndRepititiveStmt
Label3:

;OUTStmt
MOV AX,y
CALL OUTPUT
;EndOUTStmt
.exit

INPUT PROC NEAR
MOV BX,10
MOV SI,0
Lbl0:MOV AX,0
MOV AH,01H
INT 21H
CMP AL,0DH
JE Lbl1
MOV CL,AL
MOV AX,SI
MOV CH,0
MUL BX
SUB CX,30H
ADD AX,CX
MOV SI,AX
JMP Lbl0
Lbl1:MOV AX,SI
PUSH AX
PUSH DX
MOV DX,offset newline
MOV AH,09H
INT 21H
POP DX
POP AX
RET
INPUT ENDP

OUTPUT PROC NEAR
CMP AX,0
JNE Lbl11
MOV DI,1
JMP Lbl9
Lbl11:PUSH AX
MOV DI,0
MOV SI,0
LBL5:CMP AX,0
JNG LBL4
ADD DI,1
MOV BX,10
PUSH DX
MOV DX,0
DIV BX
POP DX
JMP LBL5
LBL4:POP AX
Lbl2:
SUB DI,1
CMP DI,0
JNG Lbl9
PUSH AX
MOV AX,1
MOV BX,10
PUSH DI
Lbl13:
CMP DI,0
JE Lbl12
MUL BX
SUB DI,1
JMP Lbl13
Lbl12:
POP DI
MOV BX,AX
POP AX
MOV DX,0
DIV BX
MOV SI,DX
ADD AX,30H
MOV strvar,AL
MOV DX, offset strvar
MOV AH,09H
INT 21H
MOV AX,SI
JMP Lbl2
Lbl9:
ADD AX,30H
MOV strvar,AL
MOV DX,offset strvar
MOV AH,09H
INT 21H
PUSH AX
PUSH DX
MOV DX,offset newline
MOV AH,09H
INT 21H
POP DX
POP AX
RET
OUTPUT ENDP
end
