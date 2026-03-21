package dev.trindadedev.myproject;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends Activity {
        @Override
        public void onCreate(final Bundle savedInstanceState) {
                super.onCreate(savedInstanceState);
                final TextView t = new TextView(this);
                t.setText("Hello, World");
                setContentView(t);
        }
}
