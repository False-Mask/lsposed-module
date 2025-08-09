package com.wy.lib.wytrace;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.os.Build;

import com.bytedance.shadowhook.ShadowHook;

/**
 * Description:
 *
 * @author zhou.junyou
 * Create by:Android Studio
 * Date:2022/9/22
 */
public class ArtMethodTrace {
    static {
        System.loadLibrary("perfetto-tracer");
        ShadowHook.init(new ShadowHook.ConfigBuilder()
                .setMode(ShadowHook.Mode.UNIQUE)
                .build());
    }

    /**
     * @param methodName 方法名
     * @param tid        需要抓取trace线程tid   -1表示抓取全部线程
     * @param depth      方法内部抓取层级
     * @param debug      是否打印方法耗时日志
     */

    public static void methodHookStart(String methodName, int tid, int depth, boolean debug) {
        if (Build.VERSION.SDK_INT == 33) {
            disableNterp();
        }
        methodHook(methodName, tid, depth, debug);
    }

    private static native void methodHook(String methodName, int tid, int depth, boolean debug);

    public static native void methodUnHook();

    public static native void disableNterp();

    public static native void startTrace();

    public static native void endTrace();

    public static void fix14debugApp(Context context) {
        if (Build.VERSION.SDK_INT == 34 && (context.getApplicationInfo().flags & ApplicationInfo.FLAG_DEBUGGABLE) != 0) {
            bootImageNterp();
        }
    }


    /**
     * 修复Android 14 debug包卡顿
     */

    public static native void bootImageNterp();

    public static native void deoptimizedEverything();

    /**
     * 强制走 interpretor
     */
    public static native void foreInterpretor();

}
