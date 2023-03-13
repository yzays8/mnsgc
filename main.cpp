#include <iostream>
#include <memory>

#include "vm.hpp"

void TestManualGC(VirtualMachine* vm) {
  std::cout << "Test 1: Manual GC" << std::endl;
  int objects_count = 0;

  vm->PushInt(1);
  ++objects_count;
  vm->PushInt(2);
  ++objects_count;
  VirtualMachine::Object* a = vm->PushPair(); // a->head == 1 and a->tail == 2
  ++objects_count;
  vm->PushInt(3);
  ++objects_count;
  vm->PushInt(4);
  ++objects_count;
  VirtualMachine::Object* b = vm->PushPair(); // b->head == 3 and b->tail == 4
  ++objects_count;
  a->tail = b;  // old a->tail (i.e. "2") becomes unreachable from a
  b->tail = a;  // old b->tail (i.e. "4") becomes unreachable from b
  vm->GC();     // obj "1", obj "3", obj a, obj b are living
  std::cout << "Number of objects before GC: " << objects_count << ", after GC: " << vm->num_objects << std::endl;
}

void TestAutomaticGC(VirtualMachine* vm) {
  std::cout << "Test 2: Automatic GC" << std::endl;
  VirtualMachine::Object* p1;
  VirtualMachine::Object* p2;
  bool is_p1_turn = true;

  for (int i = 0; i < 100; ++i) {
    // push a pair object at i == 2, 5, 8, ... , (3i+2)
    if (i % 3 == 2) {
      if (is_p1_turn) {
        p1 = vm->PushPair();
        is_p1_turn = false;
      } else {
        p2 = vm->PushPair();
        p1->tail = p2;
        p2->tail = p1;
        is_p1_turn = true;
      }
    } else {
      vm->PushInt(i);
    }
    std::cout << "num_objects: " << vm->num_objects << ", max_objects: " << vm->max_objects << std::endl;
  }
  vm->GC();
}

int main() {
  VirtualMachine* vm1 = new VirtualMachine;
  TestManualGC(vm1);
  delete vm1;
  putchar('\n');

  VirtualMachine* vm2 = new VirtualMachine;
  TestAutomaticGC(vm2);
  delete vm2;
}