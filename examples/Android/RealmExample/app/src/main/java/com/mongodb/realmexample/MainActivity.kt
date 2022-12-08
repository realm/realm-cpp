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

        // Pass the folder path of the app so we
        // can create a folder to store the Realm files.
        setupRealm(this.filesDir.path)

        binding.forwardButton.setOnClickListener {
            moveForward()
        }

        binding.reverseButton.setOnClickListener {
            moveBack()
        }

        binding.leftButton.setOnClickListener {
            moveLeft()
        }

        binding.rightButton.setOnClickListener {
            moveRight()
        }

        binding.colorButton.setOnClickListener {
            changeColor()
        }

        binding.resetButton.setOnClickListener {
            resetCar()
        }
    }

    fun carNotificationRecieved(changed_property: String) {
        binding.sampleText.text = changed_property
    }

    external fun setupRealm(path: String)
    // Movement methods
    external fun moveForward()
    external fun moveBack()
    external fun moveLeft()
    external fun moveRight()
    // Customization methods
    external fun changeColor()
    // Reset Car
    external fun resetCar()

    companion object {
        // Used to load the 'realmexample' library on application startup.
        init {
            System.loadLibrary("realmexample")
        }
    }
}