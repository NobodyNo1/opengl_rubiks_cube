// #include "object.hpp"
// #include <glm/glm.hpp>
// #include <iostream>
// #include <stdexcept>
// #include "stdbool.h"
// #include "object_config.hpp"
// #include <glm/gtc/matrix_transform.hpp>

// void findIntersectionForCube(
//     int cubeId,
//     glm::vec3 cubePosition,
//     glm::vec3 cubePositionWithSpread,
//     float *minimalIntersectionMagnitude,
//     SelectedCubeSide *foundMinInfo
// ) {
    
//     for (int sideIndex = 0; sideIndex < 6; sideIndex++)
//     {
//         glm::vec3 sideColor = blackColor;

//         glm::vec3 normal = glm::vec3((float)vertices[sideIndex * 24 + 3],
//                                      (float)vertices[sideIndex * 24 + 4],
//                                      (float)vertices[sideIndex * 24 + 5]);
//         glm::vec3 planePoint = glm::vec3(0.0f, 0.0f, 0.0f);

//         for (int j = 0; j < 4; j++)
//         {
//             planePoint.x += (float)vertices[sideIndex * 24 + (j)*6 + 0];
//             planePoint.y += (float)vertices[sideIndex * 24 + (j)*6 + 1];
//             planePoint.z += (float)vertices[sideIndex * 24 + (j)*6 + 2];
//         }
//         planePoint.x /= 4;
//         planePoint.y /= 4;
//         planePoint.z /= 4;

//         Cube curCube = getModel(cubeId);

//         if (!isOuterSide(sideIndex, cubePosition) || !isVisibleForView(sideIndex, cubePosition))
//             continue;
        
//         float intersectionMagnitude =
//             intersectRayPlane(normal, planePoint, cubePositionWithSpread);
//             // intersectRayPlane(normal, planePoint, cubeIdToPosition[getModel(cubeId).cubeId]);
//         printf("x:%f, y:%f, z:%f | M:%f\n", cubePosition.x, cubePosition.y, cubePosition.z, intersectionMagnitude);
//         if (intersectionMagnitude != -1.0f)
//         {
            
//             if (*minimalIntersectionMagnitude > intersectionMagnitude)
//             {
//                 *minimalIntersectionMagnitude = min(
//                     *minimalIntersectionMagnitude,
//                     intersectionMagnitude
//                 );
//                 foundMinInfo->cube = curCube;
//                 foundMinInfo->selectedSideId = sideIndex;
//                 foundMinInfo->modelIdx = cubeId;
//             }
//         }
//     }
// }

// int findIntersection(
//     float *minimalIntersectionMagnitude,
//     SelectedCubeSide *foundMinInfo
// ) {
//     float spread = SPREAD;

//     for (int i = 0; i < CUBE_SIZE; i++)
//     {
//         for (int j = 0; j < CUBE_SIZE; j++)
//         {
//             for (int k = 0; k < CUBE_SIZE; k++)
//             {
//                 // skipping center cube
//                 if (k == 1 && k == j && j == i)
//                     continue;
                
//                 int modelIdx = 3 * (3 * i + j) + k;
//                 // Cube cube = getModel(modelIdx);

//                 glm::vec3 cubePosition = glm::vec3(
//                     k - 1, j - 1, i - 1);   // cubeIdToPosition[cube.cubeId];

//                 glm::vec3 cubePositionWithSpread = spread * cubePosition;
//                 findIntersectionForCube(
//                     modelIdx, cubePosition, cubePositionWithSpread,
//                     minimalIntersectionMagnitude, foundMinInfo 
//                 );
//             }
//         }
//     }
//     if(*minimalIntersectionMagnitude!= INTERSECTION_MAGNITUDE_LIMIT){
//         //something happened
//         return 1;
//     }
//     return 0;
// }


// void handleCollision(
//     glm::mat4 projection,
//     glm::mat4 view,
//     glm::vec3 cameraPosition,
//     DragState dragState
// ) {
//     glm::mat4 inversePV = glm::inverse(projection * view);

//     // Convert mouse coordinates to normalized device coordinates (NDC)
//     // TODO: validate startX and startY, we need to confirm collision first
//     // then write down startX and startY
//     float ndcX = (2.0f * dragState.x) / WINDOW_WIDTH - 1.0f;
//     float ndcY = 1.0f - (2.0f * dragState.y) / WINDOW_HEIGHT;

//     // Perform ray-casting from mouse position in NDC space
//     glm::vec4 rayClip(ndcX, ndcY, -1.0f, 1.0f);
//     glm::vec4 rayEye = glm::inverse(projection) * rayClip;
//     rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
//     glm::vec3 rayWorld =
//         glm::normalize(glm::vec3(glm::inverse(view) * rayEye));

//     glm::vec3 rayOrigin = cameraPosition;
//     glm::vec3 rayDirection = rayWorld;

//     // printf("rayDirection: %f, %f, %f\n", rayDirection.x, rayDirection.y,
//     // rayDirection.z); end other

//     float minimalIntersectionMagnitude = INTERSECTION_MAGNITUDE_LIMIT;
//     SelectedCubeSide foundMinInfo;
//     int intersectionFound = findIntersection(&minimalIntersectionMagnitude, &foundMinInfo);
// }