using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ImuVisualize : MonoBehaviour
{
    public Transform AccelXBar;
    public Transform AccelYBar;
    public Transform AccelZBar;

    public Transform GyroAxisModel;

    public Vector3 AccelVector = Vector3.one;
    public Vector3 GyroEulorAngle = Vector3.zero;

    // Start is called before the first frame update
    void Start()
    {
        if (AccelXBar && AccelYBar && AccelZBar)
        {
            AccelXBar.GetComponent<MeshRenderer>().material.SetColor("_EmissionColor", Color.red);
            AccelYBar.GetComponent<MeshRenderer>().material.SetColor("_EmissionColor", Color.green);
            AccelZBar.GetComponent<MeshRenderer>().material.SetColor("_EmissionColor", Color.blue);
        }

        if (GyroAxisModel)
        {
            foreach (Transform child in GyroAxisModel.transform)
            {
                child.GetComponent<MeshRenderer>().material.SetColor("_EmissionColor", Color.white);
            }
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (AccelXBar && AccelYBar && AccelZBar)
        {
            AccelXBar.localScale = new Vector3(Mathf.Abs(AccelVector.x), 0.1f, 0.1f);
            AccelXBar.localPosition = new Vector3(AccelVector.x * 0.5f, 0.0f, 0.0f);

            AccelYBar.localScale = new Vector3(0.1f, Mathf.Abs(AccelVector.y), 0.1f);
            AccelYBar.localPosition = new Vector3(0.0f, AccelVector.y * 0.5f, 0.0f);

            AccelZBar.localScale = new Vector3(0.1f, 0.1f, Mathf.Abs(AccelVector.z));
            AccelZBar.localPosition = new Vector3(0.0f, 0.0f, AccelVector.z * 0.5f);
        }

        if (GyroAxisModel)
        {
            GyroAxisModel.localEulerAngles = new Vector3(GyroEulorAngle.x, GyroEulorAngle.y, GyroEulorAngle.z);
        }
    }
}
