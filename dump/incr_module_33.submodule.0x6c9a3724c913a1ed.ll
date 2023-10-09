; ModuleID = 'incr_module_33.submodule.0x6c9a3724c913a1ed.ll'
source_filename = "incr_module_33"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.Vec3.14 = type { double, double, double }

; Function Attrs: mustprogress noinline optnone uwtable
define dso_local void @_Z7reflectRK4Vec3S1_(ptr noalias sret(%struct.Vec3.14) align 8 %agg.result, ptr noundef nonnull align 8 dereferenceable(24) %incoming, ptr noundef nonnull align 8 dereferenceable(24) %normal) #0 {
entry:
  %incoming.addr = alloca ptr, align 8
  %normal.addr = alloca ptr, align 8
  %ref.tmp = alloca %struct.Vec3.14, align 8
  store ptr %incoming, ptr %incoming.addr, align 8
  store ptr %normal, ptr %normal.addr, align 8
  %0 = load ptr, ptr %incoming.addr, align 8
  %1 = load ptr, ptr %normal.addr, align 8
  %2 = load ptr, ptr %incoming.addr, align 8
  %3 = load ptr, ptr %normal.addr, align 8
  %call = call noundef double @_Z3dotRK4Vec3S1_(ptr noundef nonnull align 8 dereferenceable(24) %2, ptr noundef nonnull align 8 dereferenceable(24) %3)
  %mul = fmul double 2.000000e+00, %call
  call void @_ZmlRK4Vec3d(ptr sret(%struct.Vec3.14) align 8 %ref.tmp, ptr noundef nonnull align 8 dereferenceable(24) %1, double noundef %mul)
  call void @_ZmiRK4Vec3S1_(ptr sret(%struct.Vec3.14) align 8 %agg.result, ptr noundef nonnull align 8 dereferenceable(24) %0, ptr noundef nonnull align 8 dereferenceable(24) %ref.tmp)
  ret void
}

declare void @_ZmiRK4Vec3S1_(ptr sret(%struct.Vec3.14) align 8, ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #1

declare void @_ZmlRK4Vec3d(ptr sret(%struct.Vec3.14) align 8, ptr noundef nonnull align 8 dereferenceable(24), double noundef) #1

declare noundef double @_Z3dotRK4Vec3S1_(ptr noundef nonnull align 8 dereferenceable(24), ptr noundef nonnull align 8 dereferenceable(24)) #1

attributes #0 = { mustprogress noinline optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 18.0.0 (git@github.com:sunho/llvm-project.git 4e3adab12b509610d81502bb640accbaea39b9f9)"}
