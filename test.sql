SELECT * FROM 
  (
     SELECT Mesh.ID as Mesh_ID, Mesh.Name, SubMesh.ID as SubMesh_ID, SubMesh.IndexBufferID, SubMesh.MaterialID 
	 FROM Mesh 
	 INNER JOIN SubMesh 
	 ON Mesh.ID = SubMesh.MeshID
   ) AS SubMeshAggregate
INNER JOIN SubMeshVertexStreams ON SubMeshAggregate.SubMesh_ID = SubMeshVertexStreams.SubMeshID;