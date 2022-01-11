// This file is part of the IMP project.

#include "interp.h"
#include "program.h"

#include <iostream>



// -----------------------------------------------------------------------------
void Interp::Run()
{
  for (;;) {
    auto op = prog_.Read<Opcode>(pc_);
    switch (op) {
      case Opcode::PUSH_FUNC: {
        Push(prog_.Read<size_t>(pc_));
        continue;
      }
      case Opcode::PUSH_PROTO: {
        Push(prog_.Read<RuntimeFn>(pc_));
        continue;
      }
      case Opcode::PUSH_INT: {
        int64_t n = prog_.Read<int64_t>(pc_);
        Push(n);
        continue;
      }
      case Opcode::PEEK: {
        auto idx = prog_.Read<unsigned>(pc_);
        Push(*(stack_.rbegin() + idx));
        continue;
      }
      case Opcode::POP: {
        Pop();
        continue;
      }
      case Opcode::CALL: {
        auto callee = Pop();
        switch (callee.Kind) {
          case Value::Kind::PROTO: {
            (*callee.Val.Proto) (*this);
            continue;
          }
          case Value::Kind::ADDR: {
            Push(pc_);
            pc_ = callee.Val.Addr;
            continue;
          }
          case Value::Kind::INT: {
            throw RuntimeError("cannot call integer");
          }
        }
        continue;
      }
      case Opcode::ADD: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        long res = lhs + rhs;
        if(res < 0 && lhs >= 0 && rhs >= 0) {
          throw RuntimeError("overflow error");
        }
        
        Push(res);
        continue;
      }
      case Opcode::SUB: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        Push(lhs - rhs);
        continue;
      }
      case Opcode::MUL: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        long res = lhs * rhs;
        if(res < 0 && lhs >= 0 && rhs >= 0) {
          throw RuntimeError("overflow error");
        }
        
        Push(res);
        continue;
      }
      case Opcode::DIV: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        long res = lhs / rhs;
        if(res < 0 && lhs >= 0 && rhs >= 0) {
          throw RuntimeError("overflow error");
        }
        
        Push(res);
        continue;
      }
      case Opcode::MOD: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        
        if(rhs == 0) {
          throw RuntimeError("overflow error");
        }
        
        long res = lhs % rhs;
        Push(res);
        continue;
      }
      case Opcode::AND: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        long res = lhs && rhs;
        
        Push(res);
        continue;
      }
      case Opcode::OR: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        long res = lhs || rhs;
        
        Push(res);
        continue;
      }
      case Opcode::GREATER: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        long res = lhs < rhs;
        
        Push(res);
        continue;
      }
      case Opcode::GR_EQUAL: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        long res = lhs <= rhs;
        
        Push(res);
        continue;
      }
      case Opcode::LOWER: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        long res = lhs > rhs;
        
        Push(res);
        continue;
      }
      case Opcode::LW_EQUAL: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        long res = lhs >= rhs;

        Push(res);
        continue;
      }
      case Opcode::EQUAL_EQUAL: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        long res = lhs == rhs;
        
        Push(res);
        continue;
      }

      // >, <, <=, >=, ==... 

      case Opcode::RET: {
        auto depth = prog_.Read<unsigned>(pc_);
        auto nargs = prog_.Read<unsigned>(pc_);
        auto v = Pop();
        stack_.resize(stack_.size() - depth);
        pc_ = PopAddr();
        stack_.resize(stack_.size() - nargs);
        Push(v);
        continue;
      }
      case Opcode::JUMP_FALSE: {
        auto cond = Pop();
        auto addr = prog_.Read<size_t>(pc_);
        if (!cond) {
          pc_ = addr;
        }
        continue;
      }
      case Opcode::JUMP: {
        pc_ = prog_.Read<size_t>(pc_);
        continue;
      }
      case Opcode::STOP: {
        return;
      }
    }
  }
}
