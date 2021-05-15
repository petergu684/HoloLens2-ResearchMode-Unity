using UnityEngine;
using System.Collections;
using System.Linq;

public class ElemRenderer : MonoBehaviour
{
    Mesh mesh;

    private void Awake()
    {
    }

    // Use this for initialization
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
    }

    public void UpdateMesh(Vector3[] arrVertices, int nPointsToRender, int nPointsRendered, Color pointColor)
    {
        int nPoints;

        if (arrVertices == null)
            nPoints = 0;
        else
            nPoints = System.Math.Min(nPointsToRender, arrVertices.Length - nPointsRendered);
        nPoints = System.Math.Min(nPoints, 65535);

        Vector3[] points = arrVertices.Skip(nPointsRendered).Take(nPoints).ToArray();
        int[] indices = new int[nPoints];
        Color[] colors = new Color[nPoints];
        
        for (int i = 0; i < nPoints; i++)
        {
            //points[i] = arrVertices[nPointsRendered + i];
            indices[i] = i;
            colors[i] = pointColor;
        }

        if (mesh != null)
            Destroy(mesh);
        mesh = new Mesh();
        mesh.vertices = points;
        mesh.colors = colors;
        mesh.SetIndices(indices, MeshTopology.Points, 0);
        GetComponent<MeshFilter>().mesh = mesh;
    }
}
