class Node:
    def __init__(self, data):
        self.data = data
        self.next = None
#[9, 5, 8, 3, 4, 10, 1, 6, 2, 7]

def split(head):
    if not head or not head.next:
        return head, None
    slow = head
    fast = head.next
    while fast and fast.next:
        slow = slow.next
        fast = fast.next.next
    mid = slow.next
    slow.next = None
    return head, mid


def merge(l1, l2):
    dummy = Node(0)
    tail = dummy
    while l1 and l2:
        if l1.data < l2.data:
            tail.next = l1
            l1 = l1.next
        else:
            tail.next = l2
            l2 = l2.next
        tail = tail.next
    tail.next = l1 or l2
    return dummy.next

# merge sort 主函式
def merge_sort(head):
    if not head or not head.next:
        return head
    left, right = split(head)
    left = merge_sort(left)
    right = merge_sort(right)
    return merge(left, right)

# 測試程式
def print_list(head):
    while head:
        print(head.data, end=' ')
        head = head.next
    print()

# 建立鏈結串列
arr = [9, 5, 8, 3, 4, 10, 1, 6, 2, 7]
head = Node(arr[0])
cur = head
print(cur)
for num in arr[1:]:
    cur.next = Node(num)
    cur = cur.next

print("Before:")
print_list(head)

sorted_head = merge_sort(head) #dummy.next之後一長串，不是原本的Node.

print("After:")
print_list(sorted_head)