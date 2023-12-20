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
        SimplifyTreeConstants(tree, tree->root, &cnt, error);
        if (error->code != (int) MiddleendErrors::NONE)
            return;

        int save_cnt = cnt;

        // SimplifyExpressionNeutrals(expr, expr->root, &cnt, error);
        if (error->code != (int) MiddleendErrors::NONE)
            return;
    }
}

//------------------------------------------------------------------

static void SimplifyTreeConstants(tree_t* expr, Node* node, int* transform_cnt, error_t* error)
{
    assert(expr);
    assert(error);
    assert(transform_cnt);

    if (!node)
        return;
    if (node->left == nullptr && node->right == nullptr)
        return;

    SimplifyTreeConstants(expr, node->left, transform_cnt, error);
    if (error->code != (int) MiddleendErrors::NONE)
        return;

    SimplifyTreeConstants(expr, node->right, transform_cnt, error);
    if (error->code != (int) MiddleendErrors::NONE)
        return;

    if (node->left == nullptr)
    {
        if (node->right->type == NodeType::NUM)
        {
            UniteSubtree(expr, node, error);
            *transform_cnt++;
        }

        return;
    }

    if (node->right == nullptr)
    {
        if (node->left == NodeType::NUM)
        {
            UniteSubtree(expr, node, error);
            *transform_cnt++;
        }

        return;
    }

    if (node->left->type == NodeType::NUM && node->right->type == NodeType::NUM)
    {
        UniteSubtree(expr, node, error);
        *transform_cnt++;
        return;
    }
}

//------------------------------------------------------------------

static void UniteSubtree(tree_t* tree, Node* node, error_t* error)
{
    assert(tree);
    assert(error);

    double num = CalculateExpressionSubtree(tree, node, error);
    if (error->code != (int) MiddleendErrors::NONE)
        return;

    DestructNodes(node->left);
    DestructNodes(node->right);

    FillNode(node, nullptr, nullptr, node->parent, NodeType::NUM, {.val = num});
}

//------------------------------------------------------------------

#define DEF_OP(name, symb, priority, arg_amt, action, ...)      \
            case (Operators::name):                             \
                return action;                                  \

static double OperatorAction(const double NUMBER_1, const double NUMBER_2,
                                      const Operators operation, error_t* error)
{
    switch (operation)
    {
        #include "common/operations.h"
        default:
            error->code = (int) ExpressionErrors::UNKNOWN_OPERATION;
            return POISON;
    }
}

#undef DEF_OP

//------------------------------------------------------------------

static double CalculateExpressionSubtree(const expr_t* expr, Node* node, error_t* error)
{
    assert(expr);
    assert(error);

    if (!node) return 0;

    if (node->left == nullptr && node->right == nullptr)
    {
        if (TYPE(node) == NodeType::NUMBER)             return VAL(node);
        else if (TYPE(node) == NodeType::VARIABLE)      return expr->vars[VAR(node)].value;
        else
        {
            error->code = (int) ExpressionErrors::INVALID_EXPRESSION_FORMAT;
            return 0;
        }
    }

    double left_result  = CalculateExpressionSubtree(expr, node->left, error);
    double right_result = CalculateExpressionSubtree(expr, node->right, error);

    if (TYPE(node) != NodeType::OPERATOR)
    {
        error->code = (int) ExpressionErrors::INVALID_EXPRESSION_FORMAT;
        return 0;
    }

    double result = OperatorAction(left_result, right_result, OPT(node), error);

    if (error->code == (int) ExpressionErrors::NONE)
        return result;
    else
        return POISON;

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
