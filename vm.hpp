#pragma once

#include <array>

const int kStackMaxSize = 256;

enum ObjectType {
  OBJ_INT,
  OBJ_PAIR
};

class VirtualMachine {
 public:
  VirtualMachine();

  struct Object {
    Object(VirtualMachine* vm, ObjectType type) {
      if (vm->num_objects == vm->max_objects) {
        vm->GC(); // automatic GC
      }

      this->type = type;
      this->marked = 0;

      // the newest object is the first element of the linked list of VM
      this->next = vm->first_object_;
      vm->first_object_ = this;
      ++(vm->num_objects);
    }

    ObjectType type;
    bool marked;
    Object* next;
    union {
      // OBJ_INT
      int value;

      // OBJ_PAIR
      struct {
        Object* head;
        Object* tail;
      };
    };
  };

  void PushInt(int value);
  Object* PushPair();
  Object* PushPair(Object* head, Object* tail);
  void GC();

  ~VirtualMachine();

  int num_objects;  // the total number of currently allocated objects
  int max_objects;  // the number of objects required to trigger a GC

 private:
  void Push(Object* value);
  Object* Pop();
  void Mark(Object* object);
  void MarkAll();
  void Sweep();
  void FreeAllObjects();

  std::array<Object*, kStackMaxSize> stack_;
  int stack_ptr_;
  Object* first_object_;
};