/*
    Author: Thien K. M. Bui and Shannon Liu, extended from Professor
        Josh Davis' starter code
*/

/* Feel free to read and write this struct's members. */
typedef struct bodyBody bodyBody;
struct bodyBody {
    isoIsometry isometry;
    BodyUniforms uniforms;
    veshVesh *vesh;
    bodyBody *firstChild;
    bodyBody *nextSibling;
};

/* Like an initializer, this method sets the body into an acceptable initial 
state. Unlike an initializer, this method has no matching finalizer. */
void bodyConfigure(
        bodyBody *body, veshVesh *vesh, bodyBody *firstChild, 
        bodyBody *nextSibling) {
    float rotation[3][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
    isoSetRotation(&(body->isometry), rotation);
    float translation[3] = {0.0, 0.0, 0.0};
    isoSetTranslation(&(body->isometry), translation);
    BodyUniforms unifs = {0};
    body->uniforms = unifs;
    body->vesh = vesh;
    body->firstChild = firstChild;
    body->nextSibling = nextSibling;
}

/* Called during command buffer construction. Renders the body's vesh. The index 
must count 0, 1, 2, ... as body after body is rendered into the scene. */
void bodyRender(
        const bodyBody *body, VkCommandBuffer *cmdBuf, 
        const VkPipelineLayout *pipelineLayout, 
        const VkDescriptorSet *descriptorSet, const unifAligned *aligned, 
        int index) {
    uint32_t offset = index * aligned->alignedSize;
    vkCmdBindDescriptorSets(
        *cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipelineLayout, 0, 1, 
        descriptorSet, 1, &offset);
    veshRender(body->vesh, *cmdBuf);
}

/* Called once per animation frame. The index must match the one passed to 
bodyRender. This method loads the body's isometry into the body's uniforms and 
then copies the body's uniforms into the indexth UBO array element. */
void bodySetUniforms(bodyBody *body, const unifAligned *aligned, int index) {
    float proper[4][4];
    isoGetHomogeneous(&(body->isometry), proper);
    mat44Transpose(proper, body->uniforms.modelingT);
    BodyUniforms *bodyUnif = (BodyUniforms *)unifGetAligned(aligned, index);
    *bodyUnif = body->uniforms;
}

/* The index is the order of visitation in depth-first traveral. Renders the 
scene graph, that is rooted at the given body, including younger siblings and 
their descendants, depth-first, starting from the given index. Returns the last 
index that was used in rendering the graph. */
int bodyRenderRecursively(
        const bodyBody *body, VkCommandBuffer *cmdBuf, 
        const VkPipelineLayout *pipelineLayout, 
        const VkDescriptorSet *descriptorSet, const unifAligned *aligned, 
        int index) {
    /* NEW (KB+SL). We want inorder traversal, so children get rendered before siblings */

    bodyRender(body, cmdBuf, pipelineLayout, descriptorSet, aligned, index);
    
    if (body->firstChild != NULL){
        index = bodyRenderRecursively(body->firstChild, cmdBuf, pipelineLayout, descriptorSet, aligned, index + 1);

    }
    if (body->nextSibling != NULL){
        index = bodyRenderRecursively(body->nextSibling, cmdBuf, pipelineLayout, descriptorSet, aligned, index + 1);
    }

    return index;
}

/* The index is the order of visitation in depth-first traveral. Sets the 
uniforms for the scene graph, that is rooted at the given body, including 
younger siblings and their descendants, depth-first, starting from the given 
index. Returns the last index that was used. In more detail, this method 
computes the product of the parent's cumulative modeling isometry and the body's 
own modeling isometry. It uses that product as the cumulative modeling isometry 
to be loaded into the body's uniforms (as in bodySetUniforms). It then 
recursively causes the body's descendants (and younger siblings and their 
descendants) to load their uniforms in much the same way, so that the body's 
entire subgraph (including younger siblings) has its uniforms loaded. */
int bodySetUniformsRecursively(
        bodyBody *body, const float parent[4][4], const unifAligned *aligned, 
        int index) {
    
    float proper[4][4], bodyHomog[4][4];

    isoGetHomogeneous(&(body->isometry), bodyHomog);
    mat444Multiply(parent, bodyHomog, proper);
     
    float rot[3][3], translation[3];
    /*
    NEW (KB+SL): need to set the body isometry again
    */
   //rotation
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            rot[i][j] = proper[i][j];
        }
    }
    //translation
    for (int i = 0; i < 3; i++){
        translation[i] = proper[i][3];
    }
    isoSetRotation(&(body->isometry), rot);
    isoSetTranslation(&(body->isometry), translation);

    bodySetUniforms(body, aligned, index);

    if (body->firstChild != NULL){
        index = bodySetUniformsRecursively(body->firstChild, proper, aligned, index + 1);
    }
    if (body->nextSibling != NULL){
        index = bodySetUniformsRecursively(body->nextSibling, parent, aligned, index + 1);
    }

    return index;
}


/*
NEW (KB+SL): helper methods to add siblings/children to scene graph
*/

/* Appends the given sibling to the body's list of siblings. */
void bodyAddSibling(bodyBody *body, bodyBody *sibling) {
    body->nextSibling = sibling;
}

/* Appends the given child to the body's list of children. */
void bodyAddChild(bodyBody *body, bodyBody *child) {
    body->firstChild = child;
}
