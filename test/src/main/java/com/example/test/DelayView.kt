package com.example.test

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.util.AttributeSet
import android.view.View

class DelayView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : View(context, attrs) {

    private val paint = Paint().also {
        it.color = Color.RED
        it.textSize = 60f
    }

    override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
        // Thread.sleep(1000)
        super.onMeasure(widthMeasureSpec, heightMeasureSpec)
    }

    override fun draw(canvas: Canvas) {
        super.draw(canvas)
        canvas.drawText(System.currentTimeMillis().toString(),100f,100f,paint)
        invalidate()
    }

}