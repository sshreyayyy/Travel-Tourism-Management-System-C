#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// UI
HWND hInput, hLabel;
HWND btn[40];
int btnCount = 0;
// State
int userType = 0; // 0=none, 1=customer, 2=manager
int state = 0;
// ================= DATA =================
typedef struct Booking{
int id;
char destination[50];
int travelers;
float amount;
struct Booking* next;
}Booking;
typedef struct Customer{
int id;
char name[50];
int loyalty;
Booking* bookings;
struct Customer* next;
}Customer;
Customer* head = NULL;
Customer* current = NULL;
// Feedback
typedef struct Node{
char text[200];
struct Node* next;
}Node;
Node* feedbackHead = NULL;
// Undo stack
Booking* stack[100];
Customer* stackOwner[100];
int top = -1;
// Temp booking
char tempDestination[50];
int tempTravelers = 0;
float tempAmount = 0;
// Destinations
char* places[] = {"Goa"
"Manali"
"Dubai"
"Kerala"
,
,
,
,
"Paris"};
// ================= HELPERS =================
void clearButtons(){
for(int i=0;i<btnCount;i++)
DestroyWindow(btn[i]);
btnCount = 0;
}
HWND addBtn(HWND hwnd, char* text, int x, int y, int id){
HWND b = CreateWindow("BUTTON"
, text,
WS
_
VISIBLE|WS
CHILD,
_
x,y,170,30,
hwnd,(HMENU)id,NULL,NULL);
btn[btnCount++] = b;
return b;
}
void ClearInput(){
SetWindowText(hInput,
"");
SetFocus(hInput);
}
int genID(){
return rand()%9000 + 1000;
}
// ================= UNDO =================
void pushBooking(Customer* owner, Booking* b){
if(top<99){
stack[++top]=b;
stackOwner[top]=owner;
}
}
void undoBooking(HWND hwnd){
if(top<0){
MessageBox(hwnd,
"No booking to undo"
,
"Manager"
,MB
_
return;
OK);
}
Booking* b = stack[top];
Customer* owner = stackOwner[top];
top--;
Booking *prev=NULL,
*cur=owner->bookings;
while(cur){
if(cur==b){
if(prev) prev->next = cur->next;
else owner->bookings = cur->next;
owner->loyalty -= (int)(b->amount/100);
free(cur);
break;
}
prev=cur;
cur=cur->next;
}
MessageBox(hwnd,
"Last booking removed"
,
"Manager"
,MB
_
OK);
}
// ================= FEEDBACK =================
void addFeedback(char* text){
Node* n = (Node*)malloc(sizeof(Node));
strcpy(n->text,text);
n->next = feedbackHead;
feedbackHead = n;
}
// ================= IDS =================
#define ID
#define ID
#define ID
CUSTOMER 100
_
MANAGER 101
_
BACK 102
_
#define ID
#define ID
#define ID
#define ID
#define ID
#define ID
#define ID
#define ID
#define ID
REGISTER 200
_
LOGIN 201
_
BOOK 202
_
INVOICE 203
_
PAYMENT 204
_
VIEW
BOOKING 205
_
_
FEEDBACK 206
_
LOGOUT 207
_
LOYALTY 208
_
#define ID
#define ID
#define ID
#define ID
#define ID
#define ID
#define ID
#define ID
VIEW
CUSTOMERS 300
_
_
VIEW
ALL
BOOKINGS 301
_
_
_
UNDO 302
_
VIEW
FEEDBACK 303
_
_
TOTAL
CUSTOMERS 304
_
_
TOTAL
BOOKINGS 305
_
_
TOTAL
SALES 306
_
_
CLEAR
FEEDBACK 307
_
_
#define ID
SUBMIT 400
_
// ================= MENUS =================
void showRoleMenu(HWND hwnd){
clearButtons();
addBtn(hwnd,
"Customer"
,20,20,ID
_
CUSTOMER);
addBtn(hwnd,
"Manager"
,20,60,ID
_
MANAGER);
SetWindowText(hLabel,
"Select Role (Manager password: 1234)");
}
void showCustomerMenu(HWND hwnd){
clearButtons();
addBtn(hwnd,
"Register"
,20,120,ID
_
REGISTER);
addBtn(hwnd,
"Login"
,20,160,ID
_
LOGIN);
addBtn(hwnd,
"Book Trip"
,20,200,ID
_
BOOK);
addBtn(hwnd,
"Invoice"
,20,240,ID
_
INVOICE);
addBtn(hwnd,
"Payment"
,20,280,ID
_
PAYMENT);
addBtn(hwnd,
"My Bookings"
,20,320,ID
VIEW
_
_
BOOKING);
addBtn(hwnd,
"Feedback"
,20,360,ID
_
FEEDBACK);
addBtn(hwnd,
"Loyalty Points"
,20,400,ID
_
LOYALTY);
addBtn(hwnd,
"Logout"
,20,440,ID
_
LOGOUT);
addBtn(hwnd,
"Back"
,20,480,ID
_
BACK);
}
void showManagerMenu(HWND hwnd){
clearButtons();
addBtn(hwnd,
"View Customers"
,20,120,ID
VIEW
_
_
addBtn(hwnd,
"All Bookings"
,20,160,ID
VIEW
ALL
_
_
_
CUSTOMERS);
BOOKINGS);
addBtn(hwnd,
"Undo Booking"
,20,200,ID
_
UNDO);
addBtn(hwnd,
"View Feedback"
,20,240,ID
VIEW
_
_
FEEDBACK);
addBtn(hwnd,
"Total Customers"
,20,280,ID
TOTAL
_
_
CUSTOMERS);
addBtn(hwnd,
"Total Bookings"
,20,320,ID
TOTAL
_
_
BOOKINGS);
addBtn(hwnd,
"Total Sales"
,20,360,ID
TOTAL
_
_
SALES);
addBtn(hwnd,
"Clear Feedback"
,20,400,ID
CLEAR
_
_
FEEDBACK);
addBtn(hwnd,
"Back"
,20,440,ID
_
BACK);
}
// ================= MAIN =================
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR lpCmd,int nShow){
srand(time(NULL));
WNDCLASS wc={0};
wc.lpszClassName="TMS";
wc.hInstance=hInst;
wc.lpfnWndProc=WndProc;
wc.hbrBackground=(HBRUSH)(COLOR
_
WINDOW+1);
wc.hCursor=LoadCursor(NULL,IDC
_
ARROW);
RegisterClass(&wc);
CreateWindow("TMS"
,
"Travel Management System"
,
WS
_
OVERLAPPEDWINDOW|WS
VISIBLE,
_
100,100,900,650,
NULL,NULL,hInst,NULL);
MSG msg;
while(GetMessage(&msg,NULL,0,0)){
TranslateMessage(&msg);
DispatchMessage(&msg);
}
return 0;
}
// ================= WINDOW PROC =================
LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
switch(msg)
{
case WM
CREATE:
_
hLabel = CreateWindow("STATIC"
"Select Role"
,
,
WS
_
VISIBLE|WS
_
CHILD,250,50,400,25,hwnd,NULL,NULL,NULL);
hInput = CreateWindow("EDIT"
""
,
WS
,
_
VISIBLE|WS
_
CHILD|WS
_
BORDER,250,80,300,25,hwnd,NULL,NULL,NULL);
CreateWindow("BUTTON"
"Submit"
,
,
WS
_
VISIBLE|WS
_
CHILD,250,120,100,30,hwnd,(HMENU)ID
_
SUBMIT,NULL,NULL);
showRoleMenu(hwnd);
break;
case WM
COMMAND:
_
{
int id = LOWORD(wp);
// ===== ROLE =====
if(id==ID
_
CUSTOMER){
userType=1;
showCustomerMenu(hwnd);
}
if(id==ID
_
MANAGER){
state=10;
SetWindowText(hLabel,
"Enter Manager Password:");
ClearInput();
}
if(id==ID
_
BACK){
current=NULL;
showRoleMenu(hwnd);
}
// ===== CUSTOMER =====
if(id==ID
_
REGISTER){
state=1;
SetWindowText(hLabel,
"Enter Name:");
ClearInput();
}
if(id==ID
_
LOGIN){
state=2;
SetWindowText(hLabel,
"Enter Customer ID:");
ClearInput();
}
if(id==ID
_
BOOK){
if(!current){
MessageBox(hwnd,
"Login first"
"Error"
,
,MB
_
break;
OK);
}
char list[200]="Destinations:\n";
for(int i=0;i<5;i++){
strcat(list,places[i]);
strcat(list,
"\n");
}
MessageBox(hwnd,list,
"Packages"
,MB
_
OK);
state=3;
SetWindowText(hLabel,
"Enter Destination:");
ClearInput();
}
if(id==ID
_
INVOICE){
if(tempAmount==0){
MessageBox(hwnd,
"No booking created"
"Info"
,
,MB
_
break;
OK);
}
char msg[200];
sprintf(msg,
"Destination: %s\nTravelers: %d\nAmount: %.2f"
,
tempDestination,tempTravelers,tempAmount);
MessageBox(hwnd,msg,
"Invoice"
,MB
_
OK);
}
if(id==ID
_
PAYMENT){
if(!current || tempAmount==0){
MessageBox(hwnd,
"Create booking first"
"Error"
,
,MB
_
break;
OK);
}
Booking* b=(Booking*)malloc(sizeof(Booking));
b->id=genID();
strcpy(b->destination,tempDestination);
b->travelers=tempTravelers;
b->amount=tempAmount;
b->next=current->bookings;
current->bookings=b;
current->loyalty += (int)(tempAmount/100);
pushBooking(current,b);
char msg[100];
sprintf(msg,
"Payment Success! Booking ID: %d"
,b->id);
MessageBox(hwnd,msg,
"Success"
,MB
_
OK);
tempAmount=0;
}
if(id==ID
VIEW
_
_
BOOKING){
if(!current){
MessageBox(hwnd,
"Login first"
"Error"
,
,MB
_
break;
OK);
}
Booking* b=current->bookings;
char all[800]="My Bookings:\n";
while(b){
char t[120];
sprintf(t,
"%d - %s (%d) Rs%.0f\n"
,
b->id,b->destination,b->travelers,b->amount);
strcat(all,t);
b=b->next;
}
MessageBox(hwnd,all,
"Bookings"
,MB
_
OK);
}
if(id==ID
_
FEEDBACK){
state=5;
SetWindowText(hLabel,
"Enter Feedback:");
ClearInput();
}
if(id==ID
_
LOYALTY){
if(!current){
MessageBox(hwnd,
"Login first"
"Error"
,
,MB
_
break;
OK);
}
char msg[100];
sprintf(msg,
"Loyalty Points: %d"
,current->loyalty);
MessageBox(hwnd,msg,
"Loyalty"
,MB
_
OK);
}
if(id==ID
_
LOGOUT){
current=NULL;
MessageBox(hwnd,
"Logged out"
"Info"
,
,MB
_
OK);
}
// ===== MANAGER EXTRA =====
if(id==ID
TOTAL
_
_
CUSTOMERS){
int count=0;
Customer* c=head;
while(c){ count++; c=c->next; }
char msg[100];
sprintf(msg,
"Total Customers: %d"
,count);
MessageBox(hwnd,msg,
"Manager"
,MB
_
OK);
}
if(id==ID
TOTAL
_
_
BOOKINGS){
int count=0;
Customer* c=head;
while(c){
Booking* b=c->bookings;
while(b){ count++; b=b->next; }
c=c->next;
}
char msg[100];
sprintf(msg,
"Total Bookings: %d"
,count);
MessageBox(hwnd,msg,
"Manager"
,MB
_
OK);
}
if(id==ID
TOTAL
_
_
SALES){
float total=0;
Customer* c=head;
while(c){
Booking* b=c->bookings;
while(b){ total+=b->amount; b=b->next; }
c=c->next;
}
char msg[100];
sprintf(msg,
"Total Sales: Rs %.0f"
,total);
MessageBox(hwnd,msg,
"Manager"
,MB
_
OK);
}
if(id==ID
CLEAR
_
_
FEEDBACK){
feedbackHead=NULL;
MessageBox(hwnd,
"Feedback Cleared"
,
"Manager"
,MB
_
OK);
}
if(id==ID
_
UNDO) undoBooking(hwnd);
if(id==ID
VIEW
_
_
FEEDBACK){
Node* f = feedbackHead;
char all[1000] = "Customer Feedback:\n";
if(f == NULL){
MessageBox(hwnd,
"No feedback available"
,
"Manager"
,MB
_
return 0;
OK);
}
while(f){
strcat(all, f->text);
strcat(all,
"\n");
f = f->next;
}
MessageBox(hwnd, all,
"View Feedback"
, MB
_
OK);
}
// ===== SUBMIT =====
if(id==ID
_
SUBMIT){
char buf[200];
GetWindowText(hInput,buf,200);
if(state==10){
if(strcmp(buf,
"1234")==0){
showManagerMenu(hwnd);
}else{
MessageBox(hwnd,
"Wrong password"
"Error"
,
,MB
_
}
}
else if(state==1){
Customer* c=(Customer*)malloc(sizeof(Customer));
c->id=genID();
strcpy(c->name,buf);
c->loyalty=0;
OK);
c->bookings=NULL;
c->next=head;
head=c;
char msg[100];
sprintf(msg,
"Registered! ID: %d"
,c->id);
MessageBox(hwnd,msg,
"Success"
,MB
_
OK);
}
else if(state==2){
int cid=atoi(buf);
Customer* c=head;
while(c && c->id!=cid) c=c->next;
if(c){
current=c;
MessageBox(hwnd,
"Login Successful"
"Info"
,
,MB
_
showCustomerMenu(hwnd);
}else{
OK);
MessageBox(hwnd,
"Invalid ID"
"Error"
,
,MB
_
OK);
}
}
else if(state==3){
strcpy(tempDestination,buf);
state=4;
SetWindowText(hLabel,
"Enter Travelers:");
ClearInput();
return 0;
}
else if(state==4){
tempTravelers=atoi(buf);
tempAmount=tempTravelers*1000;
MessageBox(hwnd,
"Booking Created. Click Payment.
"
"Info"
,
,MB
_
OK);
}
else if(state==5){
addFeedback(buf);
MessageBox(hwnd,
"Feedback Saved"
"Info"
,
,MB
_
OK);
}
state=0;
ClearInput();
}
if(id==ID
VIEW
_
_
CUSTOMERS){
Customer* c = head;
char all[800] = "Customers List:\n";
if(c == NULL){
MessageBox(hwnd,
"No customers registered"
,
"Manager"
,MB
_
return 0;
OK);
}
while(c){
char temp[120];
sprintf(temp,
"ID: %d Name: %s Loyalty: %d\n"
,
c->id, c->name, c->loyalty);
strcat(all,temp);
c = c->next;
}
MessageBox(hwnd,all,
"View Customers"
,MB
_
OK);
}
if(id==ID
VIEW
ALL
_
_
_
BOOKINGS){
Customer* c = head;
char all[1200] = "All Bookings:\n";
int found = 0;
while(c){
Booking* b = c->bookings;
while(b){
char temp[150];
sprintf(temp,
"CustID:%d %s Trav:%d Amount:%.0f\n"
,
c->id,
b->destination,
b->travelers,
b->amount);
strcat(all,temp);
b = b->next;
found = 1;
}
c = c->next;
}
if(!found)
MessageBox(hwnd,
"No bookings found"
,
"Manager"
,MB
_
OK);
else
MessageBox(hwnd,all,
"All Bookings"
,MB
_
OK);
}
}
break;
case WM
DESTROY:
_
PostQuitMessage(0);
break;
}
return DefWindowProc(hwnd,msg,wp,lp);
}
