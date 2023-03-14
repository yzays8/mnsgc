#include <iostream>
#include <cstdlib>

#include "vm.hpp"

const int kInitialGcThreshold = 10;

VirtualMachine::VirtualMachine() : stack_{}, stack_ptr_{0}, first_object_{nullptr} {
  num_objects = 0;
  max_objects = kInitialGcThreshold;
}

VirtualMachine::~VirtualMachine() {
  FreeAllObjects();
}

void VirtualMachine::Push(Object* value) {
  stack_[stack_ptr_++] = value;
}

VirtualMachine::Object* VirtualMachine::Pop() {
  if (stack_ptr_ == 0) {
    std::cerr << "Stack size must be more than 0 for Push()" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  return stack_[--stack_ptr_];
}

void VirtualMachine::PushInt(int value) {
  Object* object = new Object{this, OBJ_INT};
  object->value = value;
  Push(object);
}

VirtualMachine::Object* VirtualMachine::PushPair() {
  Object* object = new Object{this, OBJ_PAIR};
  if (stack_ptr_ <= 1) {
    std::cerr << "Stack size must be more than 1 for PushPair()" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  object->tail = Pop();
  object->head = Pop();
  Push(object);
  return object;
}

VirtualMachine::Object* VirtualMachine::PushPair(Object* head, Object* tail) {
  Object* object = new Object{this, OBJ_PAIR};
  if (stack_ptr_ <= 1) {
    std::cerr << "Stack size must be more than 1 for PushPair(Object* head, Object* tail)" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  object->tail = tail;
  object->head = head;
  Push(object);
  return object;
}

void VirtualMachine::Mark(Object* object) {
  if (object->marked) return; // deal with circular references

  object->marked = 1;

  // recursively mark all reachable objects from this object
  if (object->type == OBJ_PAIR) {
    Mark(object->head);
    Mark(object->tail);
  }
}

void VirtualMachine::MarkAll() {
  for (int i = 0; i < stack_ptr_; ++i) {
    // all objects in the stack or objects pointed to by objects in the stack are marked
    Mark(stack_[i]);
  }
}

void VirtualMachine::Sweep() {
  Object** object = &first_object_;
  while (*object != nullptr) {
    if (!(*object)->marked) { // unmarked, unreachable
      Object* unreached = *object;
      *object = unreached->next;
      delete unreached;
      --num_objects;
    } else {  // marked, reachable
      (*object)->marked = 0;
      object = &(*object)->next;
    }
  }
}

void VirtualMachine::GC() {
  MarkAll();
  Sweep();
  max_objects = num_objects * 2;
}

void VirtualMachine::FreeAllObjects() {
  Object* object = first_object_;
  while (object != nullptr) {
    Object* temp = object;
    object = object->next;
    delete temp;
    --num_objects;
  }
  std::cout << "Free all objects" << std::endl;
}