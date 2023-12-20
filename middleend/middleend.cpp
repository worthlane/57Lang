#include "tree/tree.h"
#include "middleend.h"


//------------------------------------------------------------------

void OptimizeTree(tree_t* tree, error_t* error)
{
    assert(tree);
    assert(error);

    int cnt = 1;
    int simple_flag = false;
    while (cnt != 0)
    {
        cnt = 0;
        // SimplifyExpressionConstants(expr, expr->root, &cnt, error);
        if (error->code != (int) MiddleendErrors::NONE)
            return;

        int save_cnt = cnt;

        // SimplifyExpressionNeutrals(expr, expr->root, &cnt, error);
        if (error->code != (int) MiddleendErrors::NONE)
            return;
    }

    //------------------------------------------------------------------

static void SimplifyExpressionConstants(expr_t* expr, Node* node, int* transform_cnt, error_t* error)
{
    assert(expr);
    assert(error);
    assert(transform_cnt);

    if (!node)
        return;
    if (node->left == nullptr && node->right == nullptr)
        return;

    SimplifyExpressionConstants(expr, node->left, transform_cnt, error);
    if (error->code != (int) ExpressionErrors::NONE)
        return;

    SimplifyExpressionConstants(expr, node->right, transform_cnt, error);
    if (error->code != (int) ExpressionErrors::NONE)
        return;

    if (node->left == nullptr)
    {
        if (TYPE(node->right) == NodeType::NUMBER)
        {
            UniteExpressionSubtree(expr, node, error);
            *transform_cnt++;
        }

        return;
    }

    if (node->right == nullptr)
    {
        if (TYPE(node->left) == NodeType::NUMBER)
        {
            UniteExpressionSubtree(expr, node, error);
            *transform_cnt++;
        }

        return;
    }

    if (TYPE(node->left) == NodeType::NUMBER && TYPE(node->right) == NodeType::NUMBER)
    {
        UniteExpressionSubtree(expr, node, error);
        *transform_cnt++;
        return;
    }
}

//------------------------------------------------------------------

static void UniteExpressionSubtree(expr_t* expr, Node* node, error_t* error)
{
    assert(expr);
    assert(error);

    double num = CalculateExpressionSubtree(expr, node, error);
    if (error->code != (int) ExpressionErrors::NONE)
        return;

    DestructNodes(node->left);
    DestructNodes(node->right);

    FillNode(node, nullptr, nullptr, node->parent, NodeType::NUMBER, {.val = num});
}

//------------------------------------------------------------------

static void SimplifyExpressionNeutrals(tree_t* tree, Node* node, int* transform_cnt, error_t* error)
{
    assert(tree);
    assert(error);
    assert(transform_cnt);

    if (!node)
        return;
    if (node->left == nullptr && node->right == nullptr)
        return;

    SimplifyExpressionNeutrals(expr, node->left, transform_cnt, error, fp);
    if (error->code != (int) ExpressionErrors::NONE)
        return;
    SimplifyExpressionNeutrals(expr, node->right, transform_cnt, error, fp);
    if (error->code != (int) ExpressionErrors::NONE)
        return;

    if (TYPE(node) != NodeType::OPERATOR)
    {
        error->code = (int) ExpressionErrors::INVALID_EXPRESSION_FORMAT;
        return;
    }

    switch (OPT(node))
    {
        case (Operators::ADD):
            RemoveNeutralADD(expr, node, transform_cnt, error, fp);
            break;
        case (Operators::SUB):
            RemoveNeutralSUB(expr, node, transform_cnt, error, fp);
            break;
        case (Operators::MUL):
            RemoveNeutralMUL(expr, node, transform_cnt, error, fp);
            break;
        case (Operators::DIV):
            RemoveNeutralDIV(expr, node, transform_cnt, error, fp);
            break;
        case (Operators::DEG):
            RemoveNeutralDEG(expr, node, transform_cnt, error, fp);
            break;
        default:
            break;
    }
}
}
