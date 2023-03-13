#include <iostream>
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
      this->next = vm->first_object;
      vm->first_object = this;
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

  void Push(Object* value);
  void PushInt(int value);
  Object* PushPair();
  Object* PushPair(Object* head, Object* tail);
  Object* Pop();
  void Mark(Object* object);
  void MarkAll();
  void Sweep();
  void GC();
  void FreeAllObjects();

  ~VirtualMachine();

  Object* first_object;
  int num_objects;  // the total number of currently allocated objects
  int max_objects;  // the number of objects required to trigger a GC

 private:
  std::array<Object*, kStackMaxSize> stack_;
  int stack_ptr_;
};