This project provides a clean implementation of merge sort on singly linked lists in C, with an emphasis on clarity and structure to aid step-by-step debugging and visualization. 


Originally written to solve the problem of unable to use the debug tool of Spike.

## Features
- Merge sort for singly linked lists (`Node*`)
- Clear logical blocks for:
  - Splitting the list into halves
  - Merging sorted sublists
  - Recursively sorting the list
- Safe memory allocation and deallocation
- Simple `main` function that:
  - Loads input from a file
  - Performs sorting
  - Prints the sorted result


Developement enviroment: MacOS 15.5, Spike RISC-V ISA Simulator 1.1.1-dev, gcc version 14.2.0 (g04696df09)


Python file is the tool to understand how linked list merge sort works step by step.


Usage:

riscv64-unknown-elf-gcc -o main linked_list_sort_debug.c

spike pk main input.txt


Developement Notes:


因為我完全不知道merge sort，而且對linked list也不熟，因此我先用chatgpt把python寫出來。
then I convert it into c code.
procedure splitlist:
    /*
    //load *head (data) to x5
    //load *head+8 (next) to x6

    if (!head) { //beq data(x5) or = 0 to x29
        *firstHalf = *secondHalf = NULL; //字面上的意思,用sw x0,0(firsthalf)
        return;
    }
    
    Node *slow = head; //x7(slow) = head
    Node *fast = head->next; //(x6)

    while (fast && fast->next) { x6 = x0 and (x28 = 8(x6)) = x0 to x29
        slow = slow->next; lw x30 x7+8 x7 = x30 更新這個是拿來讀的
        fast = fast->next->next; lw x28 x6+8 lw x28 x28+8 x6=x28
    }

    *firstHalf = head; //firsthalf = head+0
    *secondHalf = slow->next; // sw secondhalf
    slow->next = NULL; // addi secondhalf 0,0
    */

load *head (data) to x5
load *head+8 (next) to x6
beq data(x5) or = 0 
x7(slow) = head
x28(fast) = x6 (next)

asm volatile(
    /*
    Block A (splitList), which splits the linked list into two halves
    */
    "lw x5,0(%[head])   \n\t" //*head(data)
    "lw x6,8(%[head])   \n\t" //*(head.next)
    "or x29,x5,x6       \n\t" //check head.data and head.next
    "beq x0,x29,Null    \n\t" //if (head is empty): jump to Null
    "add x7,x0,%[head]  \n\t"

    "lw x28,8(x6)       \n\t" //x28 = fast.next
    "and x29,x6,x28     \n\t" //fast && fast->next

    "loop:              \n\t"
    "beq x29,x0,out     \n\t"
    "lw x30,8(x7)       \n\t"
    "addi x7,x30,0      \n\t" //slow = slow->next
    "lw x28,8(x6)       \n\t"
    "lw x28,8(x28)      \n\t"
    "addi x6,x28,0      \n\t" //fast = fast->next->next
    "jal x0,loop        \n\t"

    "out:               \n\t"
    "lw x31,0(%[firstHalf]) \n\t" //*firsthalf
    "sw %[head],0(x31)      \n\t" //*firstHalf = head
    "lw x31,0(%[secondHalf]) \n\t" //*secondhalf
    "lw x7,8(x7)          \n\t" //*(slow.next) to x7(origin slow)
    "sw x7,0(x31)           \n\t"
    "sw x0,0(x7)            \n\t" //x0 write to address of *(slow.next)
    "jal x0,Exit            \n\t"

    "Null:             \n\t"
    "lw x31,0(%[firstHalf]) \n\t" //*firsthalf
    "sw x0,0(x31) \n\t"
    "lw x31,0(%[secondHalf]) \n\t" //*secondhalf
    "sw x0,0(x31) \n\t"
    "jal x0,Exit        \n\t"
    "Exit:"

    :[head] "+r"(head),[firstHalf] "+r"(*firstHalf),[secondHalf] "+r"(*secondHalf)
    :
    :"x5","x6","x7","x28","x29","x30","x31"
);




    /*
    // 如果其中一個為空，直接回傳另一個 address不用load除非是a->next
    x5=a;x6=b
    if (a == NULL) return b; // bne a x0 jump1; store(add) b to result; exit (important:使用%[]時就已經是對variable的記憶體做操作，不用再sw，而且也不知道address去store)
    //jump1
    if (b == NULL) return a; // bne b x0 jump2; store(add) a to result; exit
    //jump2:
    // 根據第一個節點值，初始化 result 與 tail
    if (b->data >= a->data) { // lw x28 x5(a.data); lw x29 x6(b.data); blt x29,x28, elseone
        result = a; %[result] = x5 
        tail = a; x7(tail) = x5
        a = a->next; lw x30 a->next; x5 = x30
    } else { //elseone:
        result = b; %[result] = x6
        tail = b; x7 = x6
        b = b->next; lw x30 b->next; x6 = x30
    }
    
    // 進入迴圈合併剩下的節點
    while (a != NULL && b != NULL) { loop: bne x5/x6,x0,out;
        if (b->data >= a->data) { //lw x28 x5(a.data); lw x29 x6(b.data); blt x29,x28, elsetwo
            tail->next = a; //sw x5,8(x7)
            tail = a; //x7 = x5
            a = a->next; //lw x30 a->next; x5 = x30
            //jal finishtwo
        } else { //elsetwo:
            tail->next = b; //sw x6,8(x7)
            tail = b; //x7 = x6
            b = b->next; //lw x30 a->next; x6 = x30
        }//finishtwo
    } //jal loop
    // out:
    // 把未合併完的串列接到尾端
    if (a == NULL) //bne x5,x0 elsethree
        tail->next = b; //sw x6,8(x7)
        //jal finishthree
    else //elsethree:
        tail->next = a; //sw x5,8(x7)
        //finishthree:
    */


這個程式碼完全沒有malloc，這代表我應該要用串接的方式，而不是新建一個linked list來放結果 我們直接拿a或b的heap來處理
在 RISC‑V 中沒有直接的「<=」指令，但你可以利用「bge」來達成同樣的效果。也就是說，若你要判斷 x <= y，可以改為判斷 y >= x

第一次run:
linked_list_sort.c: Assembler messages:
linked_list_sort.c:89: Error: symbol `loop' is already defined
linked_list_sort.c:107: Error: symbol `out' is already defined
linked_list_sort.c:114: Error: symbol `Exit' is already defined
第二次run:
z  0000000000000000 ra 000000000001036c sp 0000003ffffffa90 gp 0000000000024a20
tp 0000000000000000 t0 0000000000001000 t1 0000000000026820 t2 0000000000026780
s0 0000003ffffffac0 s1 0000000000000000 a0 0000000000026700 a1 0000003ffffffad8
a2 0000003ffffffad0 a3 0000000000026700 a4 0000003ffffffad8 a5 0000003ffffffad0
a6 0000000000024018 a7 0000000000000001 s2 0000000000000000 s3 0000000000000000
s4 0000000000000000 s5 0000000000000000 s6 0000000000000000 s7 0000000000000000
s8 0000000000000000 s9 0000000000000000 sA 0000000000000000 sB 0000000000000000
t3 0000000000000000 t4 ffffffffffffffff t5 0000000000026780 t6 0000000000000000
pc 0000000000010230 va/inst 0000000000000000 sr 8000000200006020
User store segfault @ 0x0000000000000000

後來發現因為firsthalf一開始是null，store *firsthalf是錯的，應該直接store進去firsthalf的記憶體位置

第三次run：
還是有segfault，我嘗試了很多debug方法，三小時後我得知一個線索，我同時跑了兩個程式，一個是我自己的asm，另一個是給chatgpt寫的可以mergesort的c code，從output中可以看到差異。
但有些時候加上debug code居然不會產生錯誤
這是一個相當常見的情況，俗稱「Heisenbug」或「觀察者效應」：
當你加上一些印資料的程式碼後，整個程式的記憶體配置或執行時序就有所改變，導致原本可能會碰到的「危險存取」不一定再馬上觸發，反而看似「被修好了」。但其實程式本身該有的邏輯或指標混亂還是存在，只是透過改變記憶體布局或寄存器用法，一時之間沒踩到雷區罷了。

其中最為重要的log如下：
iteration: 1
Now enter splitList
1
3
2
4

Firsthalf : 0x0
head this round: 0x27700

head for next round: 0x27700

iteration: 2
Now enter splitList
1
3

Firsthalf : 0x3ffffffad0
head this round: 0x27700

head for next round: 0x3f00027700

z  0000000000000000 ra 000000000001044c sp 0000003ffffffa30 gp 0000000000025c00
tp 0000000000000000 t0 0000000000027720 t1 0000000000022f68 t2 0000000000027700
s0 0000003ffffffa70 s1 0000000000000000 a0 0000003f00027700 a1 00000000000272f0
a2 000000000000001b a3 000000000000001b a4 0000000000000001 a5 0000003f00027700
a6 000000000000000a a7 0000000000000040 s2 0000000000000000 s3 0000000000000000
s4 0000000000000000 s5 0000000000000000 s6 0000000000000000 s7 0000000000000000
s8 0000000000000000 s9 0000000000000000 sA 0000000000000000 sB 0000000000000000
t3 0a0a0a0a00000000 t4 00000000000001f5 t5 0000000000000000 t6 0000000000002190
pc 000000000001039a va/inst 0000003f00027708 sr 8000000200006020
User load segfault @ 0x0000003f00027708

推測lw只讀到32位元，而現在我是在64bit的機器上跑，所以才會有3f出現
把所有lw改成ld，sw改成sd，問題就解決了
剛好node offset都是8，若是另外兩個程式也這樣替換就會出問題

check pc:riscv64-unknown-elf-objdump -d array > a.dmp 

Not head的意思是head是否為空的地址，所以檢查*head會出錯，誤會python寫的 if not head or not head.next:
如果 head 為 NULL，則先讀取 head->next 就會發生錯誤。正確順序應該先檢查 head 是否為 0，再讀取 head->next。
用 AND 指令檢查 fast 與 fast->next，但未更新檢查值:這個結果只有在進入迴圈前計算一次，但在迴圈中更新了 fast (x6) 後，並沒有重新計算 x29。因此，迴圈判斷 (beq x29,x0,out) 使用的是不正確的、停滯的值。
使用or對極端數值有風險，所以我改成分別檢查fast fast->next
