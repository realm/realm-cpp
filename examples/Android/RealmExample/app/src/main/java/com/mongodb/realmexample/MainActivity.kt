package com.mongodb.realmexample

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.MotionEvent
import android.view.View
import android.widget.TextView
import com.mongodb.realmexample.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

//        stringFromJNI(this.filesDir.path)
        // Example of a call to a native method
//        binding.sampleText.text = stringFromJNI(this.filesDir.path)

        binding.button.setOnClickListener {
            binding.sampleText.text = stringFromJNI(this.filesDir.path)
        }
    }

    fun registerErrorCallback(error: String) {
        println()
    }

    /**
     * A native method that is implemented by the 'realmexample' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(path: String): String
    external fun observe(): Void

    companion object {
        // Used to load the 'realmexample' library on application startup.
        init {
            System.loadLibrary("realmexample")
        }
    }
}