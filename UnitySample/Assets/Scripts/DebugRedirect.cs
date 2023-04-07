using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class DebugRedirect : MonoBehaviour
{
    public TextMeshPro text;

    private Queue<string> msgs = new Queue<string>();
    private static int _maxMsgCount = 8;


    void Start()
    {
        Application.logMessageReceived += LogCallbackHandler;
    }

    void LogCallbackHandler(string logString, string stackTrace, LogType type)
    {
        if (text == null) return;
        string message = string.Format("[{0}] {1}", type, logString);
        
        while (msgs.Count > _maxMsgCount - 1)
        {
            msgs.Dequeue();
        }
        msgs.Enqueue(message);

        string temp_str = "";
        foreach (string m in msgs)
        {
            temp_str += "> " + m + "\n";
        }
        text.text = temp_str;
    }

}
